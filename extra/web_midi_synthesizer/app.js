class WebSynthApp {
    constructor() {
        this.isInitialized = false;
        this.initialize();
    }

    async initialize() {
        try {
            console.log('Starting Web MIDI Synthesizer...');

            // Wait for audio engine initialization
            await this.waitForAudioEngine();

            // Initialize MIDI handling
            await this.initializeMIDI();

            // Setup global error handling
            this.setupErrorHandling();

            // Setup user interaction handling
            this.setupUserInteraction();

            // Load settings from LocalStorage
            this.loadSettingsFromStorage();

            this.isInitialized = true;
            console.log('Web MIDI Synthesizer initialization complete');

        } catch (error) {
            console.error('App initialization failed:', error);
            this.showToast('Initialization failed: ' + error.message, 'error');
        }
    }

    async waitForAudioEngine() {
        // Wait for audio engine to be ready
        return new Promise((resolve) => {
            const checkAudio = () => {
                if (window.synthEngine && window.synthEngine.audioContext) {
                    resolve();
                } else {
                    setTimeout(checkAudio, 100);
                }
            };
            checkAudio();
        });
    }

    async initializeMIDI() {
        if (!window.midiHandler) {
            throw new Error('MIDI handler not loaded');
        }

        const midiInitialized = await window.midiHandler.initialize();
        if (midiInitialized) {
            // Start listening automatically
            window.midiHandler.startListening();
            window.uiController.populateMIDIInputSelector(); // Populate MIDI device selector
            
            // Set device change callback
            window.midiHandler.onDeviceChange = () => {
                this.updateDeviceInfo();
            };

            this.updateDeviceInfo();
        }
    }

    setupErrorHandling() {
        window.addEventListener('error', (event) => {
            console.error('Global error:', event.error);
            this.showToast('An error occurred: ' + event.message, 'error');
        });

        window.addEventListener('unhandledrejection', (event) => {
            console.error('Unhandled Promise rejection:', event.reason);
            this.showToast('An error occurred: ' + event.reason, 'error');
        });
    }

    setupUserInteraction() {
        // Handle first user interaction to activate audio context
        document.addEventListener('click', () => {
            if (window.synthEngine) {
                window.synthEngine.resumeAudioContext();
            }
        }, { once: true });

        // Handle page visibility change
        document.addEventListener('visibilitychange', () => {
            if (document.hidden) {
                // Pause when page is hidden
                this.pauseAudio();
            } else {
                // Resume when page is visible
                this.resumeAudio();
            }
        });

        // Prevent context menu interference
        document.addEventListener('contextmenu', (event) => {
            if (event.target.closest('.virtual-keyboard')) {
                event.preventDefault();
            }
        });
    }

    updateDeviceInfo() {
        if (!window.midiHandler) return;

        const inputs = window.midiHandler.getInputDevices();
        const outputs = window.midiHandler.getOutputDevices();

        console.log('MIDI device info updated:');
        console.log('Input devices:', inputs);
        console.log('Output devices:', outputs);

        // Update UI status
        this.updateUIStatus(inputs.length > 0);
    }

    updateUIStatus(hasDevices) {
        const statusElement = document.getElementById('midi-status');
        if (statusElement) {
            if (hasDevices) {
                statusElement.textContent = 'MIDI Device Connected';
                statusElement.className = 'connected';
            } else {
                statusElement.textContent = 'No MIDI Device Detected';
                statusElement.className = 'disconnected';
            }
        }
    }

    pauseAudio() {
        if (window.synthEngine && window.synthEngine.channels) {
            window.synthEngine.channels.forEach(channel => {
                channel.allNotesOff();
            });
        }
    }

    resumeAudio() {
        if (window.synthEngine) {
            window.synthEngine.resumeAudioContext();
        }
    }

    showToast(message, type = 'info') {
        const toast = document.createElement('div');
        toast.className = `toast-message ${type}`;
        toast.innerHTML = `<p>${message}</p>`;

        document.body.appendChild(toast);

        setTimeout(() => {
            toast.remove();
        }, 5000);
    }

    // Apply preset
    applyPreset(presetName) {
        if (window.uiController) {
            window.uiController.applyPreset(presetName);
        }
    }

    // Export current settings
    exportSettings() {
        if (!window.synthEngine) return;

        const settings = {};
        for (let i = 1; i <= 16; i++) {
            const channelSettings = window.synthEngine.getChannelSettings(i);
            if (channelSettings) {
                settings[`channel_${i}`] = channelSettings;
            }
        }

        const blob = new Blob([JSON.stringify(settings, null, 2)], {
            type: 'application/json'
        });

        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'web-synth-settings.json';
        a.click();
        URL.revokeObjectURL(url);
    }

    // Import settings
    importSettings(file) {
        const reader = new FileReader();
        reader.onload = (event) => {
            try {
                const settings = JSON.parse(event.target.result);
                this.loadSettings(settings);
            } catch (error) {
                this.showToast('Invalid settings file format', 'error');
            }
        };
        reader.readAsText(file);
    }

    // Load settings from LocalStorage
    loadSettingsFromStorage() {
            let settings = {};
            for (let ch = 1; ch <= 16; ch++) {
                const savedSettings = localStorage.getItem(`channel_${ch}`);
                if (savedSettings && savedSettings !== 'undefined') {
                    settings[`channel_${ch}`] = JSON.parse(savedSettings);
                }
            }
            this.loadSettings(settings);
    }

    loadSettings(settings) {
        if (!window.synthEngine) return;

        Object.keys(settings).forEach(channelKey => {
            const match = channelKey.match(/channel_(\d+)/);
            if (match) {
                const channel = parseInt(match[1]);
                const channelSettings = settings[channelKey];
                window.synthEngine.updateChannelSettings(channel, channelSettings);
            }
        });

        // Reload current channel settings
        if (window.uiController) {
            window.uiController.loadChannelSettings(window.uiController.currentChannel);
        }

        this.showToast('Settings loaded', 'success');
    }

    // Get application status
    getStatus() {
        return {
            initialized: this.isInitialized,
            midiConnected: window.midiHandler && window.midiHandler.inputs.size > 0,
            audioContext: window.synthEngine && window.synthEngine.audioContext ? 
                window.synthEngine.audioContext.state : 'not-initialized',
            activeChannels: window.synthEngine && window.synthEngine.channels ? 
                Array.from(window.synthEngine.channels.keys()) : []
        };
    }

    // Destroy application
    destroy() {
        if (window.uiController) {
            window.uiController.stopSpectrumAnimation();
        }

        if (window.midiHandler) {
            window.midiHandler.destroy();
        }

        if (window.synthEngine && window.synthEngine.channels) {
            window.synthEngine.channels.forEach(channel => {
                channel.allNotesOff();
            });
        }

        this.isInitialized = false;
    }
}

// Initialize application
let app = null;

document.addEventListener('DOMContentLoaded', () => {
    // Only show overlay and bind button events, do not initialize audio/MIDI yet
    document.getElementById('start-audio').addEventListener('click', async () => {
        // True first user gesture: initialize the entire application
        window.synthEngine = new SynthEngine();
        window.midiHandler = new MIDIHandler();

        // Start audio context
        if (window.synthEngine.audioContext && window.synthEngine.audioContext.state !== 'running') {
            await window.synthEngine.audioContext.resume();
        }

        // Initialize UI and App
        app = new WebSynthApp();

        // Fade out overlay, truly hide after animation ends
        const overlay = document.getElementById('start-overlay');
        overlay.classList.add('fade-out');
        overlay.addEventListener('transitionend', () => overlay.style.display = 'none', { once: true });
    });
});

// Provide global API
window.WebSynthAPI = {
    getStatus: () => app ? app.getStatus() : { initialized: false },
    applyPreset: (preset) => app ? app.applyPreset(preset) : null,
    exportSettings: () => app ? app.exportSettings() : null,
    importSettings: (file) => app ? app.importSettings(file) : null,
    destroy: () => app ? app.destroy() : null
};

// Handle page unload
window.addEventListener('beforeunload', () => {
    if (app) {
        app.destroy();
    }
});

// Add additional shortcut features
if (typeof window !== 'undefined') {
    // Keyboard shortcuts
    document.addEventListener('keydown', (event) => {
        if (event.ctrlKey || event.metaKey) {
            switch (event.key) {
                case 'e':
                    event.preventDefault();
                    app.exportSettings();
                    break;
                case 'o':
                    event.preventDefault();
                    const input = document.createElement('input');
                    input.type = 'file';
                    input.accept = '.json';
                    input.onchange = (e) => {
                        if (e.target.files[0]) {
                            app.importSettings(e.target.files[0]);
                        }
                    };
                    input.click();
                    break;
            }
        }
    });
}