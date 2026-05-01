class UIController {
    constructor() {
        this.currentChannel = 1;
        this.virtualKeyboard = null;
        this.animationId = null;
        // Random Note: Each channel saves 24-bit mask (+1~+12, -1~-12)
        this.randomNoteMasks = {};
        for (let ch = 1; ch <= 16; ch++) this.randomNoteMasks[ch] = 1 << 12; // Default to 0
        this.initializeUI();
    }

    createRandomNotePanel() {
        const grid = document.getElementById('random-note-grid');
        if (!grid) return;
        grid.innerHTML = '';

        // Single row -12 to +12 total 25 buttons
        for (let s = -12; s <= 12; s++) {
            const btn = document.createElement('button');
            btn.className = 'random-note-btn';
            btn.dataset.semitone = s;
            btn.textContent = s === 0 ? '0' : (s > 0 ? `+${s}` : `${s}`);
            btn.addEventListener('click', () => this.toggleRandomNote(s));
            grid.appendChild(btn);
        }

        // Read current channel mask and refresh button status
        this.updateRandomNoteButtons();

        // Bind enable switch
        const enableCb = document.getElementById('random-note-enabled');
        if (enableCb) {
            enableCb.addEventListener('change', () => {
                if (window.synthEngine) {
                    window.synthEngine.setRandomNoteEnabled(this.currentChannel, enableCb.checked);
                }
            });
        }
    }

    toggleRandomNote(semi) {
        // 0 alone occupies bit12; +1~+12 occupy 13~24; -1~-12 occupy 0~11
        let bit;
        if (semi === 0) bit = 12;
        else if (semi > 0) bit = 12 + semi;
        else bit = 12 + semi;          // semi is negative, 12 + (-1) = 11 and so on
        this.randomNoteMasks[this.currentChannel] ^= (1 << bit);
        this.updateRandomNoteButtons();
        if (window.synthEngine) {
            window.synthEngine.setRandomNoteMask(this.currentChannel, this.randomNoteMasks[this.currentChannel]);
        }
    }

    updateRandomNoteButtons() {
        const mask = this.randomNoteMasks[this.currentChannel];
        document.querySelectorAll('.random-note-btn').forEach(btn => {
            const semi = parseInt(btn.dataset.semitone, 10);
            let bit;
            if (semi === 0) bit = 12;
            else if (semi > 0) bit = 12 + semi;
            else bit = 12 + semi;
            btn.classList.toggle('active', (mask & (1 << bit)) !== 0);
        });
    }

    initializeUI() {
        this.createChannelList();
        this.createVirtualKeyboard();
        this.createRandomNotePanel();
        this.setupEventListeners();
        this.populateMIDIInputSelector();
        this.startSpectrumAnimation();
        // Listen for random note light on/off
        window.addEventListener('randomNoteOn',  (e) => this.lightRandomNote(e.detail.note, true));
        window.addEventListener('randomNoteOff', (e) => this.lightRandomNote(e.detail.note, false));
    }

    createChannelList() {
        const channelList = document.getElementById('channel-list');
        if (!channelList) return;

        channelList.innerHTML = '';

        for (let i = 1; i <= 16; i++) {
            const channelItem = document.createElement('div');
            channelItem.className = 'channel-item';
            channelItem.dataset.channel = i;
            
            if (i === this.currentChannel) {
                channelItem.classList.add('active');
            }

            channelItem.innerHTML = `
                <h4>Channel ${i}</h4>
                <p class="active-notes">Active Notes: 0</p>
                <div class="channel-volume-control">
                    <input type="range" class="channel-volume-slider" min="0" max="127" value="102" data-channel="${i}">
                    <span class="volume-value">80%</span>
                </div>
            `;

            channelItem.addEventListener('click', () => {
                this.selectChannel(i);
            });

            channelList.appendChild(channelItem);
        }
    }

    createVirtualKeyboard() {
        const keyboardContainer = document.getElementById('virtual-keyboard');
        if (!keyboardContainer) return;

        keyboardContainer.innerHTML = '';
        this.virtualKeyboard = [];

        // Create piano keyboard (C3-C6)
        const startNote = 48; // C3
        const endNote = 84;   // C6
        const noteNames = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];

        // Create keyboard layout mapping
        const keyboardLayout = [
            // Order of white and black keys
            { note: 48, type: 'white', name: 'C2' },
            { note: 49, type: 'black', name: 'C#2' },
            { note: 50, type: 'white', name: 'D2' },
            { note: 51, type: 'black', name: 'D#2' },
            { note: 52, type: 'white', name: 'E2' },
            { note: 53, type: 'white', name: 'F2' },
            { note: 54, type: 'black', name: 'F#2' },
            { note: 55, type: 'white', name: 'G2' },
            { note: 56, type: 'black', name: 'G#2' },
            { note: 57, type: 'white', name: 'A2' },
            { note: 58, type: 'black', name: 'A#2' },
            { note: 59, type: 'white', name: 'B2' },
            { note: 60, type: 'white', name: 'C3' },
            { note: 61, type: 'black', name: 'C#3' },
            { note: 62, type: 'white', name: 'D3' },
            { note: 63, type: 'black', name: 'D#3' },
            { note: 64, type: 'white', name: 'E3' },
            { note: 65, type: 'white', name: 'F3' },
            { note: 66, type: 'black', name: 'F#3' },
            { note: 67, type: 'white', name: 'G3' },
            { note: 68, type: 'black', name: 'G#3' },
            { note: 69, type: 'white', name: 'A3' },
            { note: 70, type: 'black', name: 'A#3' },
            { note: 71, type: 'white', name: 'B3' },
            { note: 72, type: 'white', name: 'C4' },
            { note: 73, type: 'black', name: 'C#4' },
            { note: 74, type: 'white', name: 'D4' },
            { note: 75, type: 'black', name: 'D#4' },
            { note: 76, type: 'white', name: 'E4' },
            { note: 77, type: 'white', name: 'F4' },
            { note: 78, type: 'black', name: 'F#4' },
            { note: 79, type: 'white', name: 'G4' },
            { note: 80, type: 'black', name: 'G#4' },
            { note: 81, type: 'white', name: 'A4' },
            { note: 82, type: 'black', name: 'A#4' },
            { note: 83, type: 'white', name: 'B4' },
            { note: 84, type: 'white', name: 'C5' }
        ];

        // Create keyboard container as flex layout
        keyboardContainer.style.display = 'flex';
        keyboardContainer.style.position = 'relative';
        keyboardContainer.style.justifyContent = 'center';

        // Create all keys
        keyboardLayout.forEach((keyInfo, index) => {
            if (keyInfo.note < startNote || keyInfo.note > endNote) return;

            const key = document.createElement('div');
            key.className = `piano-key ${keyInfo.type}-key`;
            key.dataset.note = keyInfo.note;
            key.textContent = keyInfo.name;

            key.addEventListener('mousedown', (e) => {
                e.stopPropagation(); // Prevent event bubbling
                this.playVirtualNote(keyInfo.note);
            });

            key.addEventListener('mouseup', (e) => {
                e.stopPropagation();
                this.stopVirtualNote(keyInfo.note);
            });

            key.addEventListener('mouseleave', (e) => {
                e.stopPropagation();
                this.stopVirtualNote(keyInfo.note);
            });

            keyboardContainer.appendChild(key);
            this.virtualKeyboard.push({ note: keyInfo.note, element: key });
        });
    }

    setupEventListeners() {
        // Synth controls
        this.setupControlListeners();
        
        // Channel volume controls
        this.setupChannelVolumeListeners();
        
        // MIDI refresh button
        const refreshButton = document.getElementById('refresh-midi');
        if (refreshButton) {
            refreshButton.addEventListener('click', async () => {
                if (window.midiHandler) {
                    await window.midiHandler.refreshDevices();
                    this.populateMIDIInputSelector();
                    if (window.midiHandler.inputs.size > 0) {
                        window.midiHandler.startListening();
                    }
                }
            });
        }

        // MIDI input device selection
        const midiInputSelect = document.getElementById('midi-input-select');
        if (midiInputSelect) {
            midiInputSelect.addEventListener('change', (event) => {
                if (window.midiHandler) {
                    const deviceId = event.target.value;
                     window.midiHandler.startListening(deviceId === 'all' ? null : deviceId);
                }
            });
        }

        // Reset current channel button
        const resetBtn = document.getElementById('reset-channel');
        if (resetBtn) {
            resetBtn.addEventListener('click', () => this.resetCurrentChannel());
        }
        
        // Save current channel settings button
        const saveBtn = document.getElementById('save-channel');
        if (saveBtn) {
            saveBtn.addEventListener('click', () => this.saveCurrentChannel());
        }
        
        // Keyboard event support
        this.setupKeyboardEvents();
    }

    populateMIDIInputSelector() {
        const select = document.getElementById('midi-input-select');
        if (!select) return;
        select.innerHTML = ''; // Clear old options

        const devices = window.midiHandler.getInputDevices();

        if (devices.length === 0) {
            const option = document.createElement('option');
            option.textContent = 'No devices available';
            option.disabled = true;
            select.appendChild(option);
            return;
        }

        // Add an "All Devices" option
        const allOption = document.createElement('option');
        allOption.value = 'all';
        allOption.textContent = 'All Devices';
        select.appendChild(allOption);

        devices.forEach(device => {
            const option = document.createElement('option');
            option.value = device.id;
            option.textContent = device.name;
            select.appendChild(option);
        });

        // Select active device if any
        if (window.midiHandler.activeInputId) {
            select.value = window.midiHandler.activeInputId;
        } else {
            select.value = 'all'; // Default select "All Devices"
        }
    }

    setupControlListeners() {
        const controls = [
            'waveform-type', 'cutoff-freq', 'resonance',
            'attack', 'decay', 'sustain', 'release',
            'chorus-enabled', 'chorus-depth', 'chorus-rate',
            'delay-enabled', 'delay-time', 'delay-feedback',
            'reverb-enabled', 'reverb-room', 'reverb-damping'
        ];

        controls.forEach(controlId => {
            const control = document.getElementById(controlId);
            if (control) {
                control.addEventListener('change', () => {
                    this.updateChannelSettings();
                    if (controlId.includes('-enabled')) {
                        this.updateEffectControls(controlId);
                    }
                });
                control.addEventListener('input', () => {
                    this.updateDisplayValue(controlId);
                });
            }
        });
    }

    updateEffectControls(enabledControlId) {
        const effectType = enabledControlId.split('-')[0];
        const enabled = document.getElementById(enabledControlId).checked;
        
        const controls = {
            'chorus': ['chorus-depth', 'chorus-rate', 'chorus-mix'],
            'delay': ['delay-time', 'delay-feedback', 'delay-mix'],
            'reverb': ['reverb-room', 'reverb-damping', 'reverb-mix']
        };
        
        if (controls[effectType]) {
            controls[effectType].forEach(controlId => {
                const control = document.getElementById(controlId);
                if (control) {
                    control.disabled = !enabled;
                }
            });
        }
    }

    setupKeyboardEvents() {
        // Computer keyboard mapping
        const keyMap = {
            'KeyA': 60, // C3
            'KeyW': 61, // C#3
            'KeyS': 62, // D3
            'KeyE': 63, // D#3
            'KeyD': 64, // E3
            'KeyF': 65, // F3
            'KeyT': 66, // F#3
            'KeyG': 67, // G3
            'KeyY': 68, // G#3
            'KeyH': 69, // A3
            'KeyU': 70, // A#3
            'KeyJ': 71, // B3
            'KeyK': 72, // C4
            'KeyO': 73, // C#4
            'KeyL': 74, // D4
            'KeyP': 75, // D#4
            'Semicolon': 76 // E4
        };

        const pressedKeys = new Set();

        document.addEventListener('keydown', (event) => {
            if (event.repeat) return;
            
            const note = keyMap[event.code];
            if (note && !pressedKeys.has(event.code)) {
                pressedKeys.add(event.code);
                this.playVirtualNote(note);
            }
        });

        document.addEventListener('keyup', (event) => {
            const note = keyMap[event.code];
            if (note) {
                pressedKeys.delete(event.code);
                this.stopVirtualNote(note);
            }
        });
    }

    selectChannel(channelNumber) {
        this.currentChannel = channelNumber;
        
        // Update UI
        document.querySelectorAll('.channel-item').forEach(item => {
            item.classList.remove('active');
        });
        
        const selectedChannel = document.querySelector(`[data-channel="${channelNumber}"]`);
        if (selectedChannel) {
            selectedChannel.classList.add('active');
        }

        // Update current channel display
        const currentChannelDisplay = document.getElementById('current-channel');
        if (currentChannelDisplay) {
            currentChannelDisplay.textContent = channelNumber;
        }

        // Load current channel settings
        this.loadChannelSettings(this.currentChannel);
        // Random Note: refresh buttons and switch after channel change
        this.updateRandomNoteButtons();
        const enableCb = document.getElementById('random-note-enabled');
        if (enableCb && window.synthEngine) {
            enableCb.checked = window.synthEngine.getRandomNoteEnabled(channelNumber);
        }
    }

    resetCurrentChannel() {
        if (!window.synthEngine) return;
        const ch = this.currentChannel;

        // 1. Restore controls to default values (matches HTML value)
        const defaults = {
            'waveform-type': 'triangle',
            'cutoff-freq': 2000,
            'resonance': 5,
            'attack': 0.1,
            'decay': 0.3,
            'sustain': 0.7,
            'release': 0.5,
            'chorus-enabled': false,
            'chorus-depth': 0.3,
            'chorus-rate': 1.5,
            'chorus-mix': 0.5,
            'delay-enabled': false,
            'delay-time': 0.3,
            'delay-mix': 0.5,
            'delay-feedback': 0.3,
            'reverb-enabled': false,
            'reverb-room': 0.5,
            'reverb-damping': 0.4,
            'reverb-mix': 0.5,
            'random-note-enabled': false
        };

        // 1.1 Iterate over all controls and set defaults
        Object.keys(defaults).forEach(id => {
            const el = document.getElementById(id);
            if (!el) return;
            if (el.type === 'checkbox') el.checked = defaults[id];
            else el.value = defaults[id];
            this.updateDisplayValue(id);
        });

        // 2. Random Note: Only 0st (middle button, index=12) ON, rest OFF
        window.synthEngine.randomNoteMasks[ch] = 1 << 12;   // bit12 = 0st
        // 2.1 Turn all off first
        document.querySelectorAll('.random-note-btn').forEach(btn => btn.classList.remove('active'));
        // 2.2 Then light up 0st
        document.querySelectorAll('.random-note-btn')[12]?.classList.add('active');
        // 3. Effect control disabled state
        this.updateEffectControls('chorus-enabled');
        this.updateEffectControls('delay-enabled');
        this.updateEffectControls('reverb-enabled');

        // 4. Sync immediately to engine
        this.updateChannelSettings();

        // 5. Refresh random note panel UI
        if (window.randomNoteUI) window.randomNoteUI.updateGrid(ch);

        // 6. Remove channel settings from LocalStorage
        localStorage.removeItem(`channel_${ch}`);
    }

    saveCurrentChannel() {
        if (!window.synthEngine) return;
        const ch = this.currentChannel;

        // 1. Get current settings from engine
        const settings = window.synthEngine.getChannelSettings(ch);

        // 2. Save settings to LocalStorage
        localStorage.setItem(`channel_${ch}`, JSON.stringify(settings));

        // 3. Notify user of successful save
        app.showToast('Synth settings for the current channel have been saved to local storage and will automatically load on next startup.', 'success');
    }

    loadChannelSettings(channelNumber) {
        if (!window.synthEngine) return;

        const settings = window.synthEngine.getChannelSettings(channelNumber);
        if (!settings) return;

        // Update control panel
        const controls = {
            'waveform-type': settings.waveform,
            'cutoff-freq': settings.cutoffFreq,
            'resonance': settings.resonance,
            'attack': settings.attack,
            'decay': settings.decay,
            'sustain': settings.sustain,
            'release': settings.release,
            // Effect enabled states
            'chorus-enabled': settings.chorusEnabled,
            'delay-enabled': settings.delayEnabled,
            'reverb-enabled': settings.reverbEnabled,
            // Effect controls
            'chorus-depth': settings.chorusDepth,
            'chorus-rate': settings.chorusRate,
            'chorus-mix': settings.chorusMix,
            'delay-time': settings.delayTime,
            'delay-feedback': settings.delayFeedback,
            'delay-mix': settings.delayMix,
            'reverb-room': settings.reverbRoom,
            'reverb-damping': settings.reverbDamping,
            'reverb-mix': settings.reverbMix
        };

        Object.keys(controls).forEach(controlId => {
            const control = document.getElementById(controlId);
            if (control) {
                if (control.type === 'checkbox') {
                    control.checked = controls[controlId];
                } else {
                    control.value = controls[controlId];
                }
                this.updateDisplayValue(controlId);
            }
        });

        // Update effect control states
        this.updateEffectControls('chorus-enabled');
        this.updateEffectControls('delay-enabled');
        this.updateEffectControls('reverb-enabled');
    }

    updateChannelSettings() {
        if (!window.synthEngine) return;

        const settings = {
            waveform: document.getElementById('waveform-type').value,
            cutoffFreq: parseFloat(document.getElementById('cutoff-freq').value),
            resonance: parseFloat(document.getElementById('resonance').value),
            attack: parseFloat(document.getElementById('attack').value),
            decay: parseFloat(document.getElementById('decay').value),
            sustain: parseFloat(document.getElementById('sustain').value),
            release: parseFloat(document.getElementById('release').value),
            // Effect settings
            chorusEnabled: document.getElementById('chorus-enabled').checked,
            chorusDepth: parseFloat(document.getElementById('chorus-depth').value),
            chorusRate: parseFloat(document.getElementById('chorus-rate').value),
            chorusMix: parseFloat(document.getElementById('chorus-mix').value),
            delayEnabled: document.getElementById('delay-enabled').checked,
            delayTime: parseFloat(document.getElementById('delay-time').value),
            delayFeedback: parseFloat(document.getElementById('delay-feedback').value),
            delayMix: parseFloat(document.getElementById('delay-mix').value),
            reverbEnabled: document.getElementById('reverb-enabled').checked,
            reverbRoom: parseFloat(document.getElementById('reverb-room').value),
            reverbDamping: parseFloat(document.getElementById('reverb-damping').value),
            reverbMix: parseFloat(document.getElementById('reverb-mix').value)
        };

        window.synthEngine.updateChannelSettings(this.currentChannel, settings);
    }

    updateDisplayValue(controlId) {
        const control = document.getElementById(controlId);
        if (!control) return;

        const valueSpan = document.getElementById(`${controlId}-value`);
        if (!valueSpan) return;

        let value = parseFloat(control.value);
        let displayValue = value;

        switch (controlId) {
            case 'cutoff-freq':
                displayValue = `${Math.round(value)} Hz`;
                break;
            case 'resonance':
                displayValue = value.toFixed(1);
                break;
            case 'attack':
            case 'decay':
            case 'release':
                displayValue = `${value.toFixed(2)}s`;
                break;
            case 'sustain':
                displayValue = value.toFixed(2);
                break;
            case 'chorus-depth':
                displayValue = `${Math.round(value * 100)}%`;
                break;
            case 'chorus-rate':
                displayValue = `${value.toFixed(1)} Hz`;
                break;
            case 'delay-time':
                displayValue = `${value.toFixed(2)}s`;
                break;
            case 'delay-feedback':
                displayValue = `${Math.round(value * 100)}%`;
                break;
            case 'reverb-room':
                displayValue = `${Math.round(value * 100)}%`;
                break;
            case 'reverb-damping':
                displayValue = `${Math.round(value * 100)}%`;
                break;
            default:
                displayValue = value;
        }

        valueSpan.textContent = displayValue;
    }

    playVirtualNote(note) {
        if (!window.synthEngine) return;

        // Activate audio context
        window.synthEngine.resumeAudioContext();

        // Play note
        window.synthEngine.noteOn(this.currentChannel, note, 100);

        // Update virtual keyboard
        this.updateVirtualKeyboard(note, true);
    }

    stopVirtualNote(note) {
        if (!window.synthEngine) return;

        // Stop note
        window.synthEngine.noteOff(this.currentChannel, note);

        // Update virtual keyboard
        this.updateVirtualKeyboard(note, false);
    }

    updateVirtualKeyboard(note, isActive) {
        const keyElement = document.querySelector(`[data-note="${note}"]`);
        if (keyElement) {
            if (isActive) {
                keyElement.classList.add('active');
            } else {
                keyElement.classList.remove('active');
            }
        }
    }

    lightRandomNote(note, isOn) {
        const keyElement = document.querySelector(`[data-note="${note}"]`);
        if (keyElement) {
            keyElement.classList.toggle('random-active', isOn);
        }
    }

    updateChannelStatus() {
        if (!window.synthEngine) return;

        for (let i = 1; i <= 16; i++) {
            const channel = window.synthEngine.channels.get(i);
            if (channel) {
                const status = channel.getStatus();
                const channelItem = document.querySelector(`[data-channel="${i}"]`);
                const activeNotesElement = document.querySelector(`[data-channel="${i}"] .active-notes`);
                
                if (channelItem && activeNotesElement) {
                    activeNotesElement.textContent = `Active Notes: ${status.activeNotesCount}`;
                    
                    // Update highlight state based on active notes count
                    if (status.activeNotesCount > 0) {
                        channelItem.classList.add('has-active-notes');
                    } else {
                        channelItem.classList.remove('has-active-notes');
                    }
                }
            }
        }
    }

    startSpectrumAnimation() {
        const canvas = document.getElementById('spectrum-analyzer');
        if (!canvas) return;

        const ctx = canvas.getContext('2d');
        const width = canvas.width;
        const height = canvas.height;

        const animate = () => {
            if (!window.synthEngine) {
                this.animationId = requestAnimationFrame(animate);
                return;
            }

            const data = window.synthEngine.getAnalyserData();
            if (!data) {
                this.animationId = requestAnimationFrame(animate);
                return;
            }

            ctx.fillStyle = 'rgba(0, 0, 0, 0.2)';
            ctx.fillRect(0, 0, width, height);

            const barWidth = width / data.length * 2.5;
            let x = 0;

            for (let i = 0; i < data.length; i++) {
                const barHeight = (data[i] / 255) * height;

                const red = (barHeight + 25) * 2;
                const green = 250 * (i / data.length);
                const blue = 50;

                ctx.fillStyle = `rgb(${red},${green},${blue})`;
                ctx.fillRect(x, height - barHeight, barWidth, barHeight);

                x += barWidth + 1;
            }

            // Update channel status
            this.updateChannelStatus();

            this.animationId = requestAnimationFrame(animate);
        };

        animate();
    }

    stopSpectrumAnimation() {
        if (this.animationId) {
            cancelAnimationFrame(this.animationId);
            this.animationId = null;
        }
    }

    setupChannelVolumeListeners() {
        // Add event listeners to all channel volume sliders
        // Enable checkbox linked to mix slider disabled state
        document.addEventListener('change', (event) => {
            if (event.target.id === 'chorus-enabled') {
                document.getElementById('chorus-mix').disabled = !event.target.checked;
            }
            if (event.target.id === 'delay-enabled') {
                document.getElementById('delay-mix').disabled = !event.target.checked;
            }
            if (event.target.id === 'reverb-enabled') {
                document.getElementById('reverb-mix').disabled = !event.target.checked;
            }
        });

        document.addEventListener('input', (event) => {
            if (event.target.classList.contains('channel-volume-slider')) {
                const channel = parseInt(event.target.dataset.channel);
                const volume = parseInt(event.target.value);
                const percentage = Math.round((volume / 127) * 100);
                
                // Update display value
                const volumeValue = event.target.parentElement.querySelector('.volume-value');
                if (volumeValue) {
                    volumeValue.textContent = `${percentage}%`;
                }
                
                // Set channel volume
                if (window.synthEngine) {
                    window.synthEngine.setChannelVolume(channel, volume);
                }
            }
            // Effect mix real-time values
            if (event.target.id === 'chorus-mix') {
                document.getElementById('chorus-mix-value').textContent = event.target.value;
                if (window.synthEngine) window.synthEngine.setChorusMix(this.currentChannel, parseFloat(event.target.value));
            }
            if (event.target.id === 'delay-mix') {
                document.getElementById('delay-mix-value').textContent = event.target.value;
                if (window.synthEngine) window.synthEngine.setDelayMix(this.currentChannel, parseFloat(event.target.value));
            }
            if (event.target.id === 'reverb-mix') {
                document.getElementById('reverb-mix-value').textContent = event.target.value;
                if (window.synthEngine) window.synthEngine.setReverbMix(this.currentChannel, parseFloat(event.target.value));
            }
        });
    }

    updateChannelVolumeUI(channelNumber, volume) {
        // Update volume slider and display value for specified channel
        const channelItem = document.querySelector(`[data-channel="${channelNumber}"]`);
        if (channelItem) {
            const volumeSlider = channelItem.querySelector('.channel-volume-slider');
            const volumeValue = channelItem.querySelector('.volume-value');
            
            if (volumeSlider) {
                volumeSlider.value = Math.round(volume * 127);
            }
            if (volumeValue) {
                volumeValue.textContent = `${Math.round(volume * 100)}%`;
            }
        }
    }

    updateChannelPitchBendUI(channelNumber, pitchBend) {
        // Update pitch bend display for specified channel
        const channelItem = document.querySelector(`[data-channel="${channelNumber}"]`);
        if (channelItem) {
            // Find active notes element, display red if pitch bend is non-zero, otherwise default color
            const activeNotesElement = channelItem.querySelector('.active-notes');
            if (activeNotesElement) {
                const pitchBendPercent = Math.round(pitchBend * 100);
                activeNotesElement.style.color = pitchBendPercent !== 0 ? '#f00' : '#fff';
            }
        }
    }

    // Preset quick selection
    applyPreset(presetName) {
        if (!window.synthEngine) return;

        window.synthEngine.setChannelPreset(this.currentChannel, presetName);
        this.loadChannelSettings(this.currentChannel);

        // Update channel preset display
        const presetElement = document.querySelector(`[data-channel="${this.currentChannel}"] .preset-name`);
        if (presetElement) {
            const names = {
                'piano': 'Piano',
                'organ': 'Organ',
                'strings': 'Strings',
                'lead': 'Lead'
            };
            presetElement.textContent = names[presetName] || presetName;
        }
    }
}

// Export global instance
window.uiController = new UIController();