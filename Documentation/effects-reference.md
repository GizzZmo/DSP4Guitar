# DSP4Guitar — Effects Parameter Reference

This document provides the complete parameter reference for all 13 effects in the DSP4Guitar signal chain.

Effects can be **reordered at runtime** with the ▲/▼ buttons on each panel. The order is persisted with the plugin state. Each effect can be independently **enabled or disabled** with its toggle button — bypassed effects pass audio through unchanged.

---

## Signal Chain Order (default)

```
Input → [1] Noise Gate → [2] Overdrive → [3] Bitcrusher → [4] Fuzz
      → [5] Multiband Compressor → [6] Ring Modulator → [7] Auto Wah
      → [8] Phaser → [9] Chorus → [10] Tremolo → [11] Flanger
      → [12] Delay → [13] Reverb → Output
```

The order is user-reorderable and saved with presets.


---

## 1 · Noise Gate

Simple envelope-follower noise gate. Hard gates the signal when the envelope falls below the threshold.

**DSP class:** `NoiseGate` (`MultiEffectProcessor.h`)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `noiseGateOn` | off / on | off | Bypass toggle |
| Threshold | `noiseGateThreshold` | −60 – 0 dB (step 0.1) | −40 | Gate threshold in decibels. |

**Tips:** Place early in the chain when using high-gain Overdrive or Fuzz to silence residual noise.

---

## 2 · Overdrive

Soft-clipping overdrive using a `tanh` waveshaper with drive, output level and wet/dry mix.

**DSP class:** `Overdrive` (`MultiEffectProcessor.h`)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `overdriveOn` | off / on | off | Bypass toggle |
| Drive | `overdriveDrive` | 1 – 40 | 5 | Pre-clip gain. Higher values produce more saturation. |
| Level (dB) | `overdriveLevel` | −12 – +12 dB (step 0.1) | 0 | Output gain after clipping. |
| Mix | `overdriveMix` | 0.0 – 1.0 (step 0.01) | 1.0 | Wet/dry blend. |

**Tips:** Lower drive settings (2–8) give transparent boost; higher settings (15–30) produce classic overdrive grit.

---

## 3 · Bitcrusher

Reduces bit depth and/or sample rate to create lo-fi, glitchy, or chiptune-style tones.

**DSP class:** `Bitcrusher` (`MultiEffectProcessor.h`)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `bitcrusherOn` | off / on | off | Bypass toggle |
| Bit Depth | `bitcrusherDepth` | 2 – 16 bits (step 1) | 8 | Number of bits used for sample quantisation. Lower values = more distortion. |
| Downsample | `bitcrusherRate` | 1 – 100× (step 1) | 1 | Sample-hold factor (sample-rate reduction). 1 = no downsampling. |

**Tips:** Combine with Fuzz for aggressive lo-fi tones. High Bit Depth values (14–16) produce subtle warmth; low values (2–4) create extreme digital grit.

---

## 4 · Fuzz

Hard-clipping drive stage with a tone-shaping low-pass filter and output level control.

**DSP class:** `Fuzz` (`MultiEffectProcessor.h`)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `fuzzOn` | off / on | off | Bypass toggle |
| Drive | `fuzzDrive` | 1 – 100 (skewed) | 20 | Pre-clip gain. Higher values produce more intense fuzz. |
| Tone | `fuzzTone` | 0.0 – 1.0 (step 0.01) | 0.5 | Low-pass filter cutoff (0 = 500 Hz, 1 = 8 500 Hz). |
| Level (dB) | `fuzzLevel` | −20 – +20 dB (step 0.1) | 0 | Output gain in decibels after clipping and tone filter. |
| Mix | `fuzzMix` | 0.0 – 1.0 (step 0.01) | 1.0 | Wet/dry blend (0 = dry, 1 = full fuzz). |

**Tips:** Keep Level near 0 dB when mixing with other effects to avoid clipping downstream processors.

---

## 5 · Multiband Compressor

Three-band compressor with crossover filters at 300 Hz and 3 000 Hz. Each band is compressed independently before being summed back together.

**DSP class:** `MultibandCompressor` (`MultiEffectProcessor.h`)  
**Crossovers:** Low/Mid at 300 Hz · Mid/High at 3 000 Hz

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `compressorOn` | off / on | off | Bypass toggle |
| Lo Thresh | `compressorLowThresh` | −60 – 0 dB (step 0.1) | −20 | Compression threshold for the low band (< 300 Hz). |
| Mid Thresh | `compressorMidThresh` | −60 – 0 dB (step 0.1) | −20 | Compression threshold for the mid band (300 – 3 000 Hz). |
| Hi Thresh | `compressorHighThresh` | −60 – 0 dB (step 0.1) | −20 | Compression threshold for the high band (> 3 000 Hz). |
| Ratio | `compressorRatio` | 1 – 20 (step 0.1) | 4 | Compression ratio applied to all three bands. |
| Attack (ms) | `compressorAttack` | 1 – 200 ms (skewed) | 10 | Attack time in milliseconds (time to reach compression). |
| Release (ms) | `compressorRelease` | 10 – 1 000 ms (skewed) | 100 | Release time in milliseconds (time to stop compressing). |
| Makeup (dB) | `compressorMakeup` | 0 – 24 dB (step 0.1) | 0 | Post-compression output gain to compensate for level loss. |

**Tips:** Set individual band thresholds to tame specific frequency ranges — e.g., a higher (less aggressive) threshold on the low band to avoid compressing pick attack, while compressing high-frequency harshness more heavily.

---

## 6 · Ring Modulator

Multiplies the signal with a sine-wave LFO to produce metallic, robot-like, or bell tones.

**DSP class:** `RingModulator` (`MultiEffectProcessor.h`)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `ringModOn` | off / on | off | Bypass toggle |
| RM Rate | `ringModRate` | 20 – 5 000 Hz (skewed) | 440 | Frequency of the modulating sine-wave LFO in Hz. |
| RM Depth | `ringModDepth` | 0.0 – 1.0 (step 0.01) | 1.0 | Modulation depth. At 0 the signal is unaffected; at 1 full ring modulation is applied. |

**Tips:** Rates around 440 Hz produce a classic ring-modulator metallic effect. Lower rates (20–80 Hz) create a robotic warble; higher rates (1 000–5 000 Hz) produce inharmonic metallic tones.

---

## 7 · Auto Wah

An LFO-driven band-pass filter that sweeps its centre frequency to produce a wah-wah effect automatically.

**DSP class:** `WahWah` (`MultiEffectProcessor.h`)  
**Sweep range:** ±1 500 Hz around centre frequency, clamped to 200 – 4 000 Hz.

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `wahOn` | off / on | off | Bypass toggle |
| Wah Rate | `wahRate` | 0.1 – 10 Hz (step 0.01) | 2.0 | LFO sweep rate in Hz. |
| Wah Depth | `wahDepth` | 0.0 – 1.0 (step 0.01) | 0.8 | Extent of the LFO sweep. 0 = static filter; 1 = full sweep range. |
| Wah Freq | `wahFreq` | 300 – 3 000 Hz (step 1) | 1 500 | Centre frequency of the band-pass filter at rest. |
| Wah Q | `wahResonance` | 0.5 – 10 (step 0.1) | 4.0 | Filter resonance / Q-factor. Higher values = more pronounced wah peak. |
| Wah Mix | `wahMix` | 0.0 – 1.0 (step 0.01) | 1.0 | Wet/dry blend. |

**Tips:** Higher Q values (6–10) produce a more vocal wah sound. Slow rates (0.2–0.5 Hz) work well as a subtle filter effect; fast rates (4–8 Hz) create a tremolo-like flutter.

---

## 8 · Phaser

An all-pass filter network that creates frequency notches, producing sweeping phase-cancellation effects.

**JUCE class:** `juce::dsp::Phaser<float>` (JUCE DSP module)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `phaserOn` | off / on | off | Bypass toggle |
| Phaser Rate | `phaserRate` | 0.05 – 8.0 Hz (step 0.01) | 1.0 | LFO sweep rate in Hz. |
| Phaser Depth | `phaserDepth` | 0.0 – 1.0 (step 0.01) | 0.5 | Modulation depth of the LFO. |
| Phaser Fbk | `phaserFeedback` | −0.9 – +0.9 (step 0.01) | 0.3 | Feedback amount. Negative values reverse phase. |
| Phaser Mix | `phaserMix` | 0.0 – 1.0 (step 0.01) | 0.5 | Wet/dry blend. |

---

## 9 · Chorus

Adds a slightly detuned, delayed copy of the signal for a rich ensemble/widening effect.

**JUCE class:** `juce::dsp::Chorus<float>` (JUCE DSP module)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `chorusOn` | off / on | off | Bypass toggle |
| Chorus Rate | `chorusRate` | 0.1 – 10 Hz (step 0.01) | 1.0 | LFO modulation rate in Hz. |
| Chorus Depth | `chorusDepth` | 0.0 – 1.0 (step 0.01) | 0.3 | Modulation depth (controls pitch/time variation). |
| Chorus Mix | `chorusMix` | 0.0 – 1.0 (step 0.01) | 0.5 | Wet/dry blend. |

---

## 10 · Tremolo

Periodically modulates the signal amplitude using a sine-wave LFO to create a rhythmic volume pulsing effect.

**DSP class:** `Tremolo` (`MultiEffectProcessor.h`)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `tremoloOn` | off / on | off | Bypass toggle |
| Tremolo Rate | `tremoloRate` | 0.1 – 20 Hz (step 0.01) | 5.0 | LFO rate in Hz. |
| Tremolo Depth | `tremoloDepth` | 0.0 – 1.0 (step 0.01) | 0.5 | Modulation depth. At 0 there is no tremolo; at 1 the amplitude dips to silence at the LFO trough. |

**Tips:** Sync Rate to your DAW BPM (e.g., 2 Hz at 120 BPM ≈ quarter-note tremolo).


---

## 11 · Flanger

Short modulated delay with feedback, producing classic jet-flange and chorus-like sweeping effects.

**DSP class:** `Flanger` (`MultiEffectProcessor.h`)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `flangerOn` | off / on | off | Bypass toggle |
| Rate | `flangerRate` | 0.05 – 10 Hz (step 0.01) | 0.5 | LFO modulation rate. |
| Depth | `flangerDepth` | 0.0 – 1.0 (step 0.01) | 0.7 | Modulation depth (controls delay-time variation). |
| Feedback | `flangerFeedback` | −0.9 – +0.9 (step 0.01) | 0.5 | Feedback amount. Negative values reverse phase. |
| Mix | `flangerMix` | 0.0 – 1.0 (step 0.01) | 0.5 | Wet/dry blend. |

**Tips:** Slow rates (0.1–0.5 Hz) with moderate feedback give classic flanger; faster rates approach vibrato/chorus territory.

---

## 12 · Delay

A tape-style feedback delay with mix control. Maximum delay time is 2 seconds.

**JUCE class:** `juce::dsp::DelayLine<float>` with manual feedback loop (`MultiEffectProcessor.cpp`)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `delayOn` | off / on | off | Bypass toggle |
| Delay Time (ms) | `delayTime` | 1 – 2 000 ms (step 1) | 300 | Delay time in milliseconds. |
| Delay Fbk | `delayFeedback` | 0.0 – 0.98 (step 0.01) | 0.4 | Feedback amount. Values close to 0.98 produce long, decaying echoes. |
| Delay Mix | `delayMix` | 0.0 – 1.0 (step 0.01) | 0.5 | Wet/dry blend. |

**Tips:** For a dotted-eighth-note delay at 120 BPM: `delay time (ms) = (60 000 / BPM) × 0.75 = 375 ms`.

---

## 13 · Reverb

A Schroeder-style algorithmic room reverb.

**JUCE class:** `juce::dsp::Reverb` (JUCE DSP module)

| Parameter | ID | Range | Default | Description |
|-----------|----|-------|---------|-------------|
| Enable | `reverbOn` | off / on | off | Bypass toggle |
| Room Size | `reverbRoomSize` | 0.0 – 1.0 (step 0.01) | 0.5 | Virtual room size. Larger values produce longer reverb tails. |
| Damping | `reverbDamping` | 0.0 – 1.0 (step 0.01) | 0.5 | High-frequency damping. Higher values simulate more absorptive surfaces. |
| Wet Level | `reverbWetLevel` | 0.0 – 1.0 (step 0.01) | 0.33 | Level of the reverb signal (wet). |
| Dry Level | `reverbDryLevel` | 0.0 – 1.0 (step 0.01) | 0.6 | Level of the original dry signal. |
| Width | `reverbWidth` | 0.0 – 1.0 (step 0.01) | 1.0 | Stereo width of the reverb tail. 0 = mono; 1 = full stereo. |

**Tips:** The plugin reports a tail length of 2 seconds to the DAW, ensuring the reverb tail is fully rendered during export/bounce.

---

## Parameter Automation

All parameters are registered in JUCE's `AudioProcessorValueTreeState` (APVTS) and are available for DAW automation. Parameter IDs (the strings in the `ID` column above) are the keys to use when scripting or writing automation data.

Plugin state (all parameter values) is serialised to XML and persisted by the DAW project automatically via `getStateInformation` / `setStateInformation`.

---

## Preset System

Presets are saved and loaded by `PresetManager`. A preset stores the complete APVTS state as an XML snapshot. Load presets via the preset drop-down in the plugin GUI or via MIDI program-change messages.
