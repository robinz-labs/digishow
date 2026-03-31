const video = document.getElementById('video');
const statusBox = document.getElementById('status');

// Initialize WebSocket connection to DigiShow
const ws = new WebSocket("ws://127.0.0.1:50000");
ws.onopen = () => console.log("Connected to DigiShow");
ws.onclose = () => console.log("Disconnected from DigiShow");
ws.onerror = (err) => console.error("WebSocket error:", err);

// Load face-api.js models from CDN
const MODEL_URL = 'https://cdn.jsdelivr.net/gh/justadudewhohacks/face-api.js@master/weights';

Promise.all([
    faceapi.nets.tinyFaceDetector.loadFromUri(MODEL_URL),
    faceapi.nets.faceExpressionNet.loadFromUri(MODEL_URL)
]).then(startVideo).catch(err => {
    console.error("Error loading models:", err);
    statusBox.innerText = 'Error loading models.';
});

function startVideo() {
    statusBox.innerText = 'Starting camera...';
    navigator.mediaDevices.getUserMedia({ video: true })
        .then(stream => {
            video.srcObject = stream;
        })
        .catch(err => {
            console.error("Camera access error:", err);
            statusBox.innerText = 'Camera access denied or unavailable.';
        });
}

video.addEventListener('play', () => {
    statusBox.innerText = 'Detecting...';
    const canvas = document.getElementById('overlay');
    const displaySize = { width: video.width, height: video.height };
    faceapi.matchDimensions(canvas, displaySize);

    let isSmiling = false;

    setInterval(async () => {
        if (video.paused || video.ended) return;

        try {
            const detections = await faceapi.detectAllFaces(video, new faceapi.TinyFaceDetectorOptions()).withFaceExpressions();
            
            const resizedDetections = faceapi.resizeResults(detections, displaySize);
            canvas.getContext('2d').clearRect(0, 0, canvas.width, canvas.height);
            faceapi.draw.drawDetections(canvas, resizedDetections);
            
            let currentSmile = false;

            if (detections.length > 0) {
                // Check the first detected face
                const expressions = detections[0].expressions;
                
                // If 'happy' expression probability is high, consider it a smile
                if (expressions.happy > 0.6) {
                    currentSmile = true;
                    statusBox.innerText = 'Smiling! 😄';
                    statusBox.className = 'status-box smiling';
                } else {
                    currentSmile = false;
                    statusBox.innerText = 'Not Smiling 😐';
                    statusBox.className = 'status-box not-smiling';
                }
            } else {
                currentSmile = false;
                statusBox.innerText = 'No face detected';
                statusBox.className = 'status-box';
            }

            // Send WebSocket signal if smile state changes
            if (currentSmile !== isSmiling) {
                isSmiling = currentSmile;
                if (ws.readyState === WebSocket.OPEN) {
                    // Send binary signal to channel 1: dgss,1,66,0,0,<1|0>
                    const bValue = isSmiling ? 1 : 0;
                    ws.send(`dgss,1,66,0,0,${bValue}`);
                    console.log(`Sent smile state to DigiShow: ${bValue}`);
                }
            }

        } catch (err) {
            console.error("Detection error:", err);
        }
    }, 100);
});
