document.addEventListener('DOMContentLoaded', () => {
    const analogContainer = document.getElementById('analog-channels');
    const binaryContainer = document.getElementById('binary-channels');
    const noteContainer = document.getElementById('note-channels');
    const connectionStatus = document.getElementById('connection-status');
    const ws = new WebSocket('ws://127.0.0.1:50000');

    ws.onopen = () => {
        console.log('Connected to DigiShow');
        connectionStatus.textContent = '';
        connectionStatus.classList.remove('error');
    };

    ws.onclose = () => {
        console.log('Disconnected from DigiShow');
        connectionStatus.textContent = 'Disconnected from DigiShow. Please check the connection.';
        connectionStatus.classList.add('error');
    };

    ws.onerror = () => {
        console.error('WebSocket error');
        connectionStatus.textContent = 'Could not connect to DigiShow. Please ensure the server is running.';
        connectionStatus.classList.add('error');
    };

    // Create sliders for analog channels
    for (let i = 1; i <= 6; i++) {
        const channelDiv = document.createElement('div');
        channelDiv.classList.add('channel');
        channelDiv.innerHTML = `<label for="analog-${i}">Analog ${i}</label>`;

        const sliderContainer = document.createElement('div');
        sliderContainer.classList.add('slider-container');
        sliderContainer.innerHTML = `
            <div class="slider-track"></div>
            <div class="slider-thumb" id="analog-thumb-${i}"></div>
        `;
        channelDiv.appendChild(sliderContainer);
        analogContainer.appendChild(channelDiv);

        let isDragging = false;
        sliderContainer.addEventListener('mousedown', () => isDragging = true);
        document.addEventListener('mouseup', () => isDragging = false);
        document.addEventListener('mousemove', (e) => {
            if (isDragging) {
                const rect = sliderContainer.getBoundingClientRect();
                let offsetX = e.clientX - rect.left;
                offsetX = Math.max(0, Math.min(rect.width, offsetX));
                const value = Math.round((offsetX / rect.width) * 65535);
                document.getElementById(`analog-thumb-${i}`).style.left = `${(value / 65535) * 100}%`;
                ws.send(`dgss,${i},65,${value},65535,0`);
            }
        });
    }

    // Create switches for binary channels
    for (let i = 1; i <= 6; i++) {
        const channelDiv = document.createElement('div');
        channelDiv.classList.add('channel');
        channelDiv.innerHTML = `
            <label for="binary-${i}">Binary ${i}</label>
            <div class="switch-container">
                <input type="checkbox" id="binary-${i}" class="switch-input">
                <label for="binary-${i}" class="switch-label"></label>
            </div>
        `;
        binaryContainer.appendChild(channelDiv);

        document.getElementById(`binary-${i}`).addEventListener('change', (e) => {
            const value = e.target.checked ? 1 : 0;
            ws.send(`dgss,${i},66,0,0,${value}`);
        });
    }

    // Create buttons for note channels
    for (let i = 1; i <= 6; i++) {
        const channelDiv = document.createElement('div');
        channelDiv.classList.add('channel');
        channelDiv.innerHTML = `
            <label for="note-${i}">Note ${i}</label>
            <button id="note-${i}" class="custom-button">Send Note</button>
        `;
        noteContainer.appendChild(channelDiv);

        const button = document.getElementById(`note-${i}`);
        button.addEventListener('mousedown', () => ws.send(`dgss,${i},78,127,127,1`));
        button.addEventListener('mouseup', () => ws.send(`dgss,${i},78,0,127,0`));
    }

    ws.onmessage = (evt) => {
        const fields = evt.data.split(',');
        if (fields[0] !== 'dgss' || fields.length !== 6) return;

        const [, channel, signalType, aValue, , bValue] = fields.map(Number);

        if (signalType === 65) { // Analog
            const thumb = document.getElementById(`analog-thumb-${channel}`);
            if (thumb) thumb.style.left = `${(aValue / 65535) * 100}%`;
        } else if (signalType === 66) { // Binary
            const checkbox = document.getElementById(`binary-${channel}`);
            if (checkbox) checkbox.checked = bValue === 1;
        } else if (signalType === 78) { // Note
            const button = document.getElementById(`note-${channel}`);
            if (button) {
                if (bValue === 1) {
                    button.classList.add('note-on');
                } else {
                    button.classList.remove('note-on');
                }
            }
        }
    };
});
