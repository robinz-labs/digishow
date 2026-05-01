class MIDIHandler {
    constructor() {
        this.midiAccess = null;
        this.inputs = new Map();
        this.outputs = new Map();
        this.onDeviceChange = null;
        this.isListening = false;
        this.activeInputId = null; // Track currently active input device ID
        this.boundHandleMIDIMessage = this.handleMIDIMessage.bind(this); // Pre-bind this
    }

    async initialize() {
        try {
            if (!navigator.requestMIDIAccess) {
                throw new Error('Your browser does not support the Web MIDI API');
            }

            this.midiAccess = await navigator.requestMIDIAccess({
                sysex: false
            });

            this.setupEventListeners();
            this.scanDevices();
            
            console.log('MIDI handling module initialization complete');
            return true;
        } catch (error) {
            console.error('MIDI initialization failed:', error);
            this.updateMIDIStatus('Browser does not support MIDI', false);
            return false;
        }
    }

    setupEventListeners() {
        if (!this.midiAccess) return;

        // Listen for MIDI device connect/disconnect
        this.midiAccess.addEventListener('statechange', (event) => {
            console.log('MIDI device status changed:', event.port);
            this.scanDevices();
            if (this.onDeviceChange) {
                this.onDeviceChange();
            }
        });
    }

    scanDevices() {
        this.inputs.clear();
        this.outputs.clear();

        // Scan input devices
        for (let input of this.midiAccess.inputs.values()) {
            this.inputs.set(input.id, input);
            console.log('Found MIDI input device:', input.name);
        }

        // Scan output devices
        for (let output of this.midiAccess.outputs.values()) {
            this.outputs.set(output.id, output);
            console.log('Found MIDI output device:', output.name);
        }

        this.updateMIDIStatus();
    }

    startListening(deviceId = null) {
        this.stopListening(); // Stop all listening first

        if (deviceId) {
            const input = this.inputs.get(deviceId);
            if (input) {
                input.addEventListener('midimessage', this.boundHandleMIDIMessage);
                this.activeInputId = deviceId;
                console.log(`Started listening to specific MIDI input: ${input.name}`);
            } else {
                console.error(`MIDI input device with ID ${deviceId} not found`);
                return;
            }
        } else {
            // If no device ID is specified, listen to all devices
            this.inputs.forEach((input, id) => {
                input.addEventListener('midimessage', this.boundHandleMIDIMessage);
                console.log(`Started listening to MIDI input: ${input.name}`);
            });
            this.activeInputId = 'all';
        }

        this.isListening = true;
        console.log('MIDI listening started');
    }

    stopListening() {
        if (!this.isListening) return;

        if (this.activeInputId && this.activeInputId !== 'all') {
            const input = this.inputs.get(this.activeInputId);
            if (input) {
                input.removeEventListener('midimessage', this.boundHandleMIDIMessage);
            }
        } else {
            this.inputs.forEach((input, id) => {
                input.removeEventListener('midimessage', this.boundHandleMIDIMessage);
            });
        }

        this.isListening = false;
        this.activeInputId = null;
        console.log('MIDI listening stopped');
    }

    handleMIDIMessage(event) {
        const [command, data1, data2] = event.data;
        const channel = (command & 0x0F) + 1; // MIDI channel numbers start from 1
        const messageType = command & 0xF0;

        // Ensure audio context is activated
        if (window.synthEngine) {
            window.synthEngine.resumeAudioContext();
        }

        switch (messageType) {
            case 0x90: // Note On
                if (data2 > 0) {
                    this.handleNoteOn(channel, data1, data2);
                } else {
                    this.handleNoteOff(channel, data1);
                }
                break;

            case 0x80: // Note Off
                this.handleNoteOff(channel, data1);
                break;

            case 0xB0: // Control Change
                this.handleControlChange(channel, data1, data2);
                break;

            case 0xC0: // Program Change
                this.handleProgramChange(channel, data1);
                break;

            case 0xE0: // Pitch Bend
                this.handlePitchBend(channel, data1, data2);
                break;

            default:
                console.log('Unhandled MIDI message:', {
                    command: command.toString(16),
                    channel,
                    data1,
                    data2
                });
        }
    }

    handleNoteOn(channel, note, velocity) {
        console.log(`Channel ${channel}: Note On - Note ${note}, Velocity ${velocity}`);
        
        if (window.synthEngine) {
            window.synthEngine.noteOn(channel, note, velocity);
        }

        // Update UI
        this.updateVirtualKeyboard(note, true);
    }

    handleNoteOff(channel, note) {
        console.log(`Channel ${channel}: Note Off - Note ${note}`);
        
        if (window.synthEngine) {
            window.synthEngine.noteOff(channel, note);
        }

        // Update UI
        this.updateVirtualKeyboard(note, false);
    }

    handleControlChange(channel, controller, value) {
        console.log(`Channel ${channel}: Control Change - Controller ${controller}, Value ${value}`);
        
        // Handle common MIDI controllers
        switch (controller) {
            case 1: // Modulation Wheel
                console.log('Modulation Wheel:', value);
                break;
            case 7: // Volume
                console.log(`Channel ${channel}: Volume Controller - Value: ${value}`);
                if (window.synthEngine) {
                    window.synthEngine.setChannelVolume(channel, value);
                }
                break;
            case 64: // Sustain Pedal
                console.log('Sustain Pedal:', value >= 64 ? 'On' : 'Off');
                break;
            case 123: // All Notes Off
                if (window.synthEngine) {
                    window.synthEngine.channels.forEach(channel => {
                        channel.allNotesOff();
                    });
                }
                break;
        }
    }

    handleProgramChange(channel, program) {
        console.log(`Channel ${channel}: Program Change - Program ${program}`);
        
        // Map MIDI program number to synth preset
        const presets = ['piano', 'organ', 'strings', 'lead'];
        const presetIndex = program % presets.length;
        const preset = presets[presetIndex];
        
        if (window.synthEngine) {
            window.synthEngine.setChannelPreset(channel, preset);
        }

        // Update UI
        this.updateChannelUI(channel, preset);
    }

    handlePitchBend(channel, lsb, msb) {
        // MIDI Pitch Bend message format: 0xE0 + channel, LSB, MSB
        // Combine LSB and MSB into 14-bit value (0-16383)
        const pitchBendValue = (msb << 7) | lsb;
        // Convert to -1 to +1 range, 8192 is center (no pitch bend)
        const pitchBend = (pitchBendValue - 8192) / 8192;
        
        console.log(`Channel ${channel}: Pitch Bend - Value: ${pitchBendValue} (${pitchBend.toFixed(3)})`);
        
        if (window.synthEngine) {
            window.synthEngine.setChannelPitchBend(channel, pitchBend);
        }
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

    updateChannelUI(channel, preset) {
        const channelElement = document.querySelector(`[data-channel="${channel}"]`);
        if (channelElement) {
            const presetText = channelElement.querySelector('.preset-name');
            if (presetText) {
                presetText.textContent = this.getPresetDisplayName(preset);
            }
        }
    }

    getPresetDisplayName(preset) {
        const names = {
            'piano': 'Piano',
            'organ': 'Organ',
            'strings': 'Strings',
            'lead': 'Lead'
        };
        return names[preset] || preset;
    }

    updateMIDIStatus(message = null, connected = null) {
        const statusElement = document.getElementById('midi-status');
        if (!statusElement) return;

        if (message) {
            statusElement.textContent = message;
            statusElement.className = connected ? 'connected' : 'disconnected';
        } else if (this.inputs.size > 0) {
            statusElement.textContent = `Connected to ${this.inputs.size} MIDI device(s)`;
            statusElement.className = 'connected';
        } else {
            statusElement.textContent = 'No MIDI device found';
            statusElement.className = 'disconnected';
        }
    }

    getInputDevices() {
        return Array.from(this.inputs.values()).map(input => ({
            id: input.id,
            name: input.name,
            manufacturer: input.manufacturer,
            version: input.version
        }));
    }

    getOutputDevices() {
        return Array.from(this.outputs.values()).map(output => ({
            id: output.id,
            name: output.name,
            manufacturer: output.manufacturer,
            version: output.version
        }));
    }

    // Send MIDI message to output device
    sendMIDIMessage(outputId, command, channel, data1, data2 = 0) {
        const output = this.outputs.get(outputId);
        if (!output) {
            console.error('MIDI output device not found:', outputId);
            return;
        }

        const message = [command | (channel - 1), data1, data2];
        output.send(message);
    }

    // Refresh device list
    async refreshDevices() {
        if (this.midiAccess) {
            this.scanDevices();
            return true;
        }
        return false;
    }

    // Clean up resources
    destroy() {
        this.stopListening();
        this.inputs.clear();
        this.outputs.clear();
        this.midiAccess = null;
    }
}

// Export global instance
window.midiHandler = new MIDIHandler();