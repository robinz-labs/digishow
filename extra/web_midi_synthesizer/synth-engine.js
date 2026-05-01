class SynthEngine {
    constructor() {
        this.audioContext = null;
        this.masterGain = null;
        this.channels = new Map();
        this.analyser = null;
        // Random Note: channel masks/enabled/mapping stacks
        this.randomNoteMasks = {};
        this.randomNoteEnabled = {};
        this.randomNoteMaps = {}; // channel -> { inputNote: [outputNotes...] }
        for (let ch = 1; ch <= 16; ch++) {
            this.randomNoteMasks[ch] = 1 << 12; // Default to 0
            this.randomNoteEnabled[ch] = false;
            this.randomNoteMaps[ch] = new Map();
        }
        this.initializeAudio();
    }

    async initializeAudio() {
        try {
            this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
            this.masterGain = this.audioContext.createGain();
            this.masterGain.connect(this.audioContext.destination);
            this.masterGain.gain.value = 0.7;

            // Create spectrum analyzer
            this.analyser = this.audioContext.createAnalyser();
            this.analyser.fftSize = 2048;
            this.masterGain.connect(this.analyser);

            // Initialize 16 MIDI channels
            for (let i = 1; i <= 16; i++) {
                this.channels.set(i, new SynthChannel(i, this.audioContext, this.masterGain));
            }

            console.log('Audio engine initialization complete');
        } catch (error) {
            console.error('Audio engine initialization failed:', error);
        }
    }

    async resumeAudioContext() {
        if (this.audioContext && this.audioContext.state === 'suspended') {
            await this.audioContext.resume();
        }
    }

    noteOn(channel, note, velocity) {
        if (!this.channels.has(channel)) return;
        // Random Note branch
        if (this.randomNoteEnabled[channel]) {
            const mask = this.randomNoteMasks[channel];
            const candidates = [];
            // 25-bit mask: 0..11→-12..-1, 12→0, 13..24→+1..+12
            for (let i = 0; i < 25; i++) {
                if (mask & (1 << i)) {
                    let semi;
                    if (i < 12) semi = -(12 - i);
                    else if (i === 12) semi = 0;
                    else semi = i - 12;
                    candidates.push(note + semi);
                }
            }
            if (candidates.length) {
                const outNote = candidates[Math.floor(Math.random() * candidates.length)];
                // Record mapping, support multiple inputs mapped to same output
                const map = this.randomNoteMaps[channel];
                if (!map.has(note)) map.set(note, []);
                map.get(note).push(outNote);
                // Actually play the outNote
                this.channels.get(channel).noteOn(outNote, velocity);
                // Notify UI to highlight random note
                window.dispatchEvent(new CustomEvent('randomNoteOn', { detail: { channel, note: outNote } }));
                return;
            }
        }
        // Original path
        this.channels.get(channel).noteOn(note, velocity);
    }

    noteOff(channel, note) {
        if (!this.channels.has(channel)) return;
        const map = this.randomNoteMaps[channel];
        if (this.randomNoteEnabled[channel] && map.has(note)) {
            const outs = map.get(note);
            if (outs && outs.length) {
                const outNote = outs.shift(); // Extract earliest mapping
                this.channels.get(channel).noteOff(outNote);
                // Notify UI to turn off random note
                window.dispatchEvent(new CustomEvent('randomNoteOff', { detail: { channel, note: outNote } }));
                if (outs.length === 0) map.delete(note); // Clear array if empty
                return;
            }
        }
        // Original path
        this.channels.get(channel).noteOff(note);
    }

    setChannelPreset(channel, preset) {
        if (this.channels.has(channel)) {
            this.channels.get(channel).setPreset(preset);
        }
    }

    setChannelVolume(channel, volume) {
        if (this.channels.has(channel)) {
            this.channels.get(channel).setVolume(volume);
        }
    }

    setChannelPitchBend(channel, pitchBend) {
        if (this.channels.has(channel)) {
            this.channels.get(channel).setPitchBend(pitchBend);
        }
    }

    // Random Note: UI set mask/enabled
    setRandomNoteMask(channel, mask25) {
        this.randomNoteMasks[channel] = mask25 >>> 0; // Ensure unsigned 25-bit
    }
    setRandomNoteEnabled(channel, enabled) {
        this.randomNoteEnabled[channel] = enabled;
    }
    getRandomNoteEnabled(channel) {
        return this.randomNoteEnabled[channel];
    }

    setChannelSettings(channel, settings) {
        if (this.channels.has(channel)) {
            this.channels.get(channel).setSettings(settings);
        }
    }

    getChannelSettings(channel) {
        if (this.channels.has(channel)) {
            return this.channels.get(channel).getSettings();
        }
        return null;
    }

    updateChannelSettings(channel, settings) {
        if (this.channels.has(channel)) {
            this.channels.get(channel).updateSettings(settings);
        }
    }

    getAnalyserData() {
        if (!this.analyser) return null;
        
        const bufferLength = this.analyser.frequencyBinCount;
        const dataArray = new Uint8Array(bufferLength);
        this.analyser.getByteFrequencyData(dataArray);
        return dataArray;
    }

    // Proxy: effect mix ratio
    setChorusMix(channel, mix) {
        if (this.channels.has(channel)) {
            this.channels.get(channel).setChorusMix(mix);
        }
    }
    setDelayMix(channel, mix) {
        if (this.channels.has(channel)) {
            this.channels.get(channel).setDelayMix(mix);
        }
    }
    setReverbMix(channel, mix) {
        if (this.channels.has(channel)) {
            this.channels.get(channel).setReverbMix(mix);
        }
    }
}

class SynthChannel {
    constructor(channelNumber, audioContext, outputNode) {
        this.channelNumber = channelNumber;
        this.audioContext = audioContext;
        this.outputNode = outputNode;
        this.activeNotes = new Map();
        
        // Default synth settings
        this.settings = {
            waveform: 'triangle',
            cutoffFreq: 2000,
            resonance: 5,
            attack: 0.1,
            decay: 0.3,
            sustain: 0.7,
            release: 0.5,
            volume: 0.8,
            pitchBend: 0, // Pitch bend value (-1 to +1)
            pitchBendRange: 2, // Pitch bend range (semitones)
            // Effect settings
            chorusEnabled: false,
            chorusDepth: 0.3,
            chorusRate: 1.5,
            chorusMix: 0.5,
            delayEnabled: false,
            delayTime: 0.3,
            delayFeedback: 0.3,
            delayMix: 0.5,
            reverbEnabled: false,
            reverbRoom: 0.5,
            reverbDamping: 0.4,
            reverbMix: 0.5
        };

        this.channelGain = this.audioContext.createGain();
        this.filter = this.audioContext.createBiquadFilter();
        
        // Effect nodes
        console.log('Creating effect nodes...');
        this.chorus = this.createChorus();
        this.delay = this.createDelay();
        this.reverb = this.createReverb();
        console.log('Effect nodes created');
        
        this.filter.type = 'lowpass';
        this.filter.frequency.value = this.settings.cutoffFreq;
        this.filter.Q.value = this.settings.resonance;
        
        // Audio routing initialization
        this.channelGain.connect(this.filter);
        
        // Bypass path (used when effects are disabled)
        this.bypassGain = this.audioContext.createGain();
        this.bypassGain.gain.value = 1; // Default bypass effects
        
        // Ensure initial connection - connect directly to bypass
        this.filter.connect(this.bypassGain);
        this.bypassGain.connect(this.outputNode);
        
        // Initialize effect states
        this.updateEffectRouting();
        
        this.channelGain.gain.value = this.settings.volume;
    }

    // Create chorus effect
    createChorus() {
        const input = this.audioContext.createGain();
        const output = this.audioContext.createGain();
        const delay = this.audioContext.createDelay(0.1);
        const lfo = this.audioContext.createOscillator();
        const lfoGain = this.audioContext.createGain();
        const dryGain = this.audioContext.createGain();
        const wetGain = this.audioContext.createGain();
        
        delay.delayTime.value = 0.02;
        lfo.type = 'sine';
        lfo.frequency.value = this.settings.chorusRate;
        lfoGain.gain.value = this.settings.chorusDepth * 0.01;
        
        lfo.connect(lfoGain);
        lfoGain.connect(delay.delayTime);
        
        // Dry/wet splitting
        dryGain.gain.value = 0;
        wetGain.gain.value = 0;
        
        input.connect(dryGain);
        input.connect(delay);
        delay.connect(wetGain);
        dryGain.connect(output);
        wetGain.connect(output);
        
        lfo.start();
        
        console.log('Chorus effect created');
        console.log('Chorus input:', input);
        console.log('Chorus output:', output);
        
        return { input, output, delay, lfo, lfoGain, dryGain, wetGain };
    }

    // Create delay effect
    createDelay() {
        const input = this.audioContext.createGain();
        const output = this.audioContext.createGain();
        const delay = this.audioContext.createDelay(1);
        const feedback = this.audioContext.createGain();
        const filter = this.audioContext.createBiquadFilter();
        const dryGain = this.audioContext.createGain();
        const wetGain = this.audioContext.createGain();
        
        delay.delayTime.value = this.settings.delayTime;
        feedback.gain.value = this.settings.delayFeedback;
        filter.type = 'lowpass';
        filter.frequency.value = 2000;
        
        // Dry/wet splitting
        dryGain.gain.value = 0;
        wetGain.gain.value = 0;
        
        input.connect(dryGain);
        input.connect(delay);
        delay.connect(filter);
        filter.connect(feedback);
        feedback.connect(delay);
        delay.connect(wetGain);
        dryGain.connect(output);
        wetGain.connect(output);
        
        console.log('Delay effect created');
        console.log('Delay input:', input);
        console.log('Delay output:', output);
        
        return { input, output, delay, feedback, filter, dryGain, wetGain };
    }

    // Create reverb effect
    createReverb() {
        const input = this.audioContext.createGain();
        const output = this.audioContext.createGain();
        const convolver = this.audioContext.createConvolver();
        const dryGain = this.audioContext.createGain();
        const wetGain = this.audioContext.createGain();
        
        // Create impulse response
        const impulseBuffer = this.createImpulseResponse(
            this.settings.reverbRoom, 
            this.settings.reverbDamping
        );
        convolver.buffer = impulseBuffer;
        
        // Initially set all gains to 0 (mute)
        dryGain.gain.value = 0;
        wetGain.gain.value = 0;
        
        input.connect(dryGain);
        input.connect(convolver);
        convolver.connect(wetGain);
        dryGain.connect(output);
        wetGain.connect(output);
        
        console.log('Reverb effect created');
        console.log('Reverb input:', input);
        console.log('Reverb output:', output);
        
        return { input, output, convolver, dryGain, wetGain };
    }

    // Create impulse response
    createImpulseResponse(roomSize, damping) {
        const length = this.audioContext.sampleRate * 2;
        const impulse = this.audioContext.createBuffer(2, length, this.audioContext.sampleRate);
        
        for (let channel = 0; channel < 2; channel++) {
            const channelData = impulse.getChannelData(channel);
            for (let i = 0; i < length; i++) {
                const decay = Math.pow(1 - damping, i / this.audioContext.sampleRate);
                channelData[i] = (Math.random() * 2 - 1) * decay * roomSize;
            }
        }
        
        return impulse;
    }

    noteOn(note, velocity) {
        const baseFreq = this.midiNoteToFreq(note);
        const velocityGain = velocity / 127;
        
        // Apply pitch bend effect
        const pitchBendSemitones = this.settings.pitchBend * this.settings.pitchBendRange;
        const freq = baseFreq * Math.pow(2, pitchBendSemitones / 12);

        // If note is already playing, stop it first
        if (this.activeNotes.has(note)) {
            this.noteOff(note);
        }

        // Create oscillator
        const oscillator = this.audioContext.createOscillator();
        const gainNode = this.audioContext.createGain();
        
        oscillator.type = this.settings.waveform;
        oscillator.frequency.value = freq;
        
        // Set gain node
        gainNode.gain.value = 0;
        
        // Connect audio graph
        oscillator.connect(gainNode);
        gainNode.connect(this.channelGain);
        
        // Apply ADSR envelope
        const now = this.audioContext.currentTime;
        const attackTime = now + this.settings.attack;
        const decayTime = attackTime + this.settings.decay;
        let sustainLevel = this.settings.sustain * velocityGain;

        // Fix exponentialRampToValueAtTime throwing error when value is 0
        if (sustainLevel <= 0) {
            sustainLevel = 0.0001; 
        }

        gainNode.gain.setValueAtTime(0, now);
        gainNode.gain.linearRampToValueAtTime(velocityGain, attackTime);
        gainNode.gain.exponentialRampToValueAtTime(sustainLevel, decayTime);
        
        oscillator.start(now);
        
        this.activeNotes.set(note, {
            oscillator,
            gainNode,
            velocityGain
        });
    }

    noteOff(note) {
        if (!this.activeNotes.has(note)) return;
        
        const { oscillator, gainNode, velocityGain } = this.activeNotes.get(note);
        const now = this.audioContext.currentTime;
        
        // Apply release phase
        const releaseTime = now + this.settings.release;
        gainNode.gain.exponentialRampToValueAtTime(0.001, releaseTime);
        
        // Stop oscillator after release completes
        oscillator.stop(releaseTime);
        
        this.activeNotes.delete(note);
    }

    setPreset(preset) {
        switch (preset) {
            case 'piano':
                this.settings = {
                    waveform: 'triangle',
                    cutoffFreq: 2000,
                    resonance: 1,
                    attack: 0.01,
                    decay: 0.3,
                    sustain: 0.8,
                    release: 0.3,
                    volume: 0.8
                };
                break;
            case 'organ':
                this.settings = {
                    waveform: 'square',
                    cutoffFreq: 1000,
                    resonance: 5,
                    attack: 0.05,
                    decay: 0.1,
                    sustain: 1.0,
                    release: 0.1,
                    volume: 0.7
                };
                break;
            case 'strings':
                this.settings = {
                    waveform: 'sawtooth',
                    cutoffFreq: 800,
                    resonance: 2,
                    attack: 0.5,
                    decay: 0.3,
                    sustain: 0.6,
                    release: 1.0,
                    volume: 0.9
                };
                break;
            case 'lead':
                this.settings = {
                    waveform: 'sawtooth',
                    cutoffFreq: 3000,
                    resonance: 8,
                    attack: 0.01,
                    decay: 0.1,
                    sustain: 0.9,
                    release: 0.2,
                    volume: 0.8
                };
                break;
        }
        this.updateFilter();
    }

    updateSettings(settings) {
        Object.assign(this.settings, settings);
        this.updateFilter();
        
        // Update effect parameters and routing
        this.updateEffectRouting();
        this.updateChorusSettings();
        this.updateDelaySettings();
        this.updateReverbSettings();
    }

    updateEffectRouting() {
        const anyEffectEnabled = this.settings.chorusEnabled || 
                               this.settings.delayEnabled || 
                               this.settings.reverbEnabled;
        
        console.log('=== Update Effect Routing ===');
        console.log('Chorus enabled:', this.settings.chorusEnabled);
        console.log('Delay enabled:', this.settings.delayEnabled);
        console.log('Reverb enabled:', this.settings.reverbEnabled);
        console.log('Any effect enabled:', anyEffectEnabled);
        
        try {
            // Safely disconnect existing connections (ignore errors)
            try { this.filter.disconnect(); } catch(e) {}
            try { this.bypassGain.disconnect(); } catch(e) {}
            
            // Reset all effect node gains to 0 (mute)
            // Note: input.gain stays at 1, use dry/wetGain for mix/mute
            this.chorus.input.gain.value = 1;
            this.chorus.output.gain.value = 1;
            this.chorus.dryGain.gain.value = 0;
            this.chorus.wetGain.gain.value = 0;

            this.delay.input.gain.value = 1;
            this.delay.output.gain.value = 1;
            this.delay.dryGain.gain.value = 0;
            this.delay.wetGain.gain.value = 0;

            this.reverb.input.gain.value = 1;
            this.reverb.output.gain.value = 1;
            this.reverb.dryGain.gain.value = 0;
            this.reverb.wetGain.gain.value = 0;
            
            if (anyEffectEnabled) {
                console.log('Enabling effect chain...');
                
                // Disable bypass
                this.bypassGain.gain.value = 0;
                
                // Always start from filter
                let currentNode = this.filter;
                
                // Chorus effect
                if (this.settings.chorusEnabled) {
                    console.log('Enabling chorus effect');
                    this.chorus.dryGain.gain.value = 1 - this.settings.chorusMix;
                    this.chorus.wetGain.gain.value = this.settings.chorusMix;
                    currentNode.connect(this.chorus.input);
                    currentNode = this.chorus.output;
                } else {
                    // Disabled: dry signal passthrough, do not break chain
                    this.chorus.dryGain.gain.value = 0;
                    this.chorus.wetGain.gain.value = 0;
                    // currentNode stays same and passes through
                }
                
                // Delay effect
                if (this.settings.delayEnabled) {
                    console.log('Enabling delay effect');
                    this.delay.dryGain.gain.value = 1 - this.settings.delayMix;
                    this.delay.wetGain.gain.value = this.settings.delayMix;
                    currentNode.connect(this.delay.input);
                    currentNode = this.delay.output;
                } else {
                    this.delay.dryGain.gain.value = 0;
                    this.delay.wetGain.gain.value = 0;
                    // Dry signal passthrough, do not break chain
                }
                
                // Reverb effect
                if (this.settings.reverbEnabled) {
                    console.log('Enabling reverb effect');
                    this.reverb.input.gain.value = 1;
                    this.reverb.output.gain.value = 1;
                    this.reverb.dryGain.gain.value = 1 - this.settings.reverbMix;
                    this.reverb.wetGain.gain.value = this.settings.reverbMix;
                    currentNode.connect(this.reverb.input);
                    currentNode = this.reverb.output;
                } else {
                    this.reverb.dryGain.gain.value = 0;
                    this.reverb.wetGain.gain.value = 0;
                    // Dry signal passthrough, do not break chain
                }
                
                // Connect to final output
                currentNode.connect(this.outputNode);
                
            } else {
                console.log('Disabling effects, using bypass');
                // Enable bypass
                this.bypassGain.gain.value = 1;
                this.filter.connect(this.bypassGain);
                this.bypassGain.connect(this.outputNode);
            }
              
            console.log('=== Routing update complete ===');
            
        } catch (error) {
            console.error('Effect routing update failed:', error);
            
            // Emergency recovery
            console.log('Executing emergency recovery');
            try {
                this.bypassGain.gain.value = 1;
                this.filter.connect(this.bypassGain);
                this.bypassGain.connect(this.outputNode);
                console.log('Emergency recovery successful');
            } catch(e) {
                console.error('Emergency recovery also failed:', e);
            }
        }
    }

    updateChorusSettings() {
        this.chorus.lfo.frequency.value = this.settings.chorusRate;
        this.chorus.lfoGain.gain.value = this.settings.chorusDepth * 0.01;
    }

    updateDelaySettings() {
        this.delay.delay.delayTime.value = this.settings.delayTime;
        this.delay.feedback.gain.value = this.settings.delayFeedback;
    }

    updateReverbSettings() {
        // Recreate reverb impulse response
        const newImpulse = this.createImpulseResponse(
            this.settings.reverbRoom,
            this.settings.reverbDamping
        );
        this.reverb.convolver.buffer = newImpulse;
    }

    updateFilter() {
        this.filter.frequency.value = this.settings.cutoffFreq;
        this.filter.Q.value = this.settings.resonance;
        this.channelGain.gain.value = this.settings.volume;
    }

    setVolume(volume) {
        // Convert MIDI CC value (0-127) to volume (0-1)
        const normalizedVolume = Math.max(0, Math.min(1, volume / 127));
        this.settings.volume = normalizedVolume;
        this.channelGain.gain.value = normalizedVolume;
        
        console.log(`Channel ${this.channelNumber}: Volume set to ${Math.round(normalizedVolume * 100)}%`);
        
        // Trigger UI update event
        if (window.uiController) {
            window.uiController.updateChannelVolumeUI(this.channelNumber, normalizedVolume);
        }
    }

    setPitchBend(pitchBend) {
        // pitchBend range: -1 to +1
        this.settings.pitchBend = Math.max(-1, Math.min(1, pitchBend));
        
        console.log(`Channel ${this.channelNumber}: Pitch bend set to ${(this.settings.pitchBend * 100).toFixed(1)}%`);
        
        // Update frequencies of all active notes
        this.activeNotes.forEach((noteData, note) => {
            const baseFreq = this.midiNoteToFreq(note);
            const pitchBendSemitones = this.settings.pitchBend * this.settings.pitchBendRange;
            const newFreq = baseFreq * Math.pow(2, pitchBendSemitones / 12);
            
            noteData.oscillator.frequency.setValueAtTime(newFreq, this.audioContext.currentTime);
        });
        
        // Trigger UI update event
        if (window.uiController) {
            window.uiController.updateChannelPitchBendUI(this.channelNumber, this.settings.pitchBend);
        }
    }

    getSettings() {
        return { ...this.settings };
    }

    midiNoteToFreq(note) {
        return 440 * Math.pow(2, (note - 69) / 12);
    }

    // Clear all active notes
    allNotesOff() {
        this.activeNotes.forEach((_, note) => {
            this.noteOff(note);
        });
    }

    // Get channel status
    getStatus() {
        return {
            channelNumber: this.channelNumber,
            activeNotesCount: this.activeNotes.size,
            settings: this.getSettings()
        };
    }

    // Effect mix interface
    setChorusMix(mix) {
        mix = Number(mix);
        if (!Number.isFinite(mix)) return;   // Reject NaN/Infinity
        this.settings.chorusMix = Math.max(0, Math.min(1, mix));
        this.updateEffectRouting();
    }
    setDelayMix(mix) {
        mix = Number(mix);
        if (!Number.isFinite(mix)) return;
        this.settings.delayMix = Math.max(0, Math.min(1, mix));
        this.updateEffectRouting();
    }
    setReverbMix(mix) {
        mix = Number(mix);
        if (!Number.isFinite(mix)) return;
        this.settings.reverbMix = Math.max(0, Math.min(1, mix));
        this.updateEffectRouting();
    }
}

// Export global instance
window.synthEngine = new SynthEngine();