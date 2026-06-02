; =============================================================================
; synth.orc — Instrument collection for idyl live composition
; #include this file inside the <CsInstruments> section of main.csd
;
; Assumes the following are already defined in main.csd:
;   sr, ksmps, 0dbfs, nchnls
;   opcode mix_ch, 0, ai
;   gamaster[]
;
; Instrument list
; ────────────────────────────────────────────────────────────────────
;  lushpad    — smooth harmonic synth, soft attack, rich harmonics
;               for long just-intonation chords
;
;  grainsharp — noisy IDM, FM + filtered noise, gritty grains
;  metalsharp — metallic IDM, FM with inharmonic ratios, sharp ring
;  purecut    — sine-ish IDM, clean + precise, hard transient click
;
;  vaporpad   — vaporwave chorus pad, heavy reverb, very slow attack
;  vapordream — vaporwave retro-piano attack melting into dreamy sustain
; =============================================================================


; ─────────────────────────────────────────────────────────────────────────────
; lushpad — Strong harmonic synth for long just-intonation chords
; ─────────────────────────────────────────────────────────────────────────────
; p4 = amplitude (0–1)
; p5 = frequency (Hz)
; p6 = output channel (1 or 2)
; p7 = brightness (0–1): 0 = closed/dark, 1 = moderately open  [default 0.5]
;
; Character: thick sub-anchored body with four detuned sawtooths. Saturation
; before the filter adds harmonic density without adding brightness. Two slow
; LFOs (filter cutoff + amplitude) make it breathe — never static. The filter
; stays in the low-mid range even at maximum brightness: presence over air.
; ─────────────────────────────────────────────────────────────────────────────
instr lushpad
    iamp    init p4
    ifreq   init p5
    ich init p6 
    ibright init (p7 == 0 ? 0.5 : p7)

    ; Slow attack, full sustain
    iatt = 0.5
    idec = 2
    aenv linsegr 0, iatt, 1, p3 - iatt, 1, idec, 0.0001

    ; Very slow pitch drift on the outer voices — barely perceptible, 0.05 Hz
    ; Makes unisons feel alive rather than mechanically static
    kdetune lfo 0.0016, 0.05, 0

    ; === OSCILLATORS ===
    ; Sub sine — the foundation, intentionally the loudest layer
    asub  oscili 0.5, ifreq * 0.5, -1

    ; Core pair: very tight detuning — focused, not washy
    asaw1 vco2 0.80, ifreq * 0.9994
    asaw2 vco2 0.80, ifreq * 1.0006

    ; Width pair: moderate detuning + slow drift
    asaw3 vco2 0.50, ifreq * (1.0055 + kdetune)
    asaw4 vco2 0.50, ifreq * (0.9945 - kdetune)

    ; Weight the mix toward sub and fundamental — low-side emphasis
    amix = asub * 0.40 + (asaw1 + asaw2) * 0.26 + (asaw3 + asaw4) * 0.16

    ; === SATURATION ===
    ; Pre-filter soft clip adds harmonic "meat" that the filter then shapes.
    ; Drive set so the saws bulk up without folding into harshness.
    amix = tanh(amix * 2.8) / tanh(2.8)

    ; === FILTER ===
    ; Base cutoff sits low: barely above 2× fundamental at ibright=0,
    ; reaching ~3× fundamental + 900 Hz at ibright=1. Never opens wide.
    icutbase = ifreq * 2.1
    icuttop  = icutbase + ibright * 900 + 150

    ; Filter slowly rises from icutbase to icuttop over the first 3 seconds
    kcut linseg icutbase, 3, icuttop, p3 - 3, icuttop

    ; Slow LFO wobbles the cutoff at 0.11 Hz — ±12% of base — gives breathing
    kcutmod lfo icutbase * 0.12, 0.11, 0
    kcut = kcut + kcutmod

    ; Add some flanging 
    aflange = flanger(amix, rspline:a(0.002, 0.003, 0.1, 0.5) , 0.75)

    ; Moderate resonance: adds a tonal bump at cutoff — character, not squeal
    afilt moogvcf2 aflange, kcut, 0.38

    ; === AMPLITUDE BREATHING ===
    ; Independent slow sine at 0.07 Hz — ±5% — the pad feels alive, not frozen
    kbreath lfo 0.05, 0.07, 0
    kenv_mod = 1 + kbreath

    aout = afilt * aenv * kenv_mod * iamp * 1.2
    //mix_ch aout, 1
    //mix_ch aout, 2

    mix_ch(aout, ich)
    if(ich == $NCH) then 
      mix_ch(aout, ich - 1) 
    else
      mix_ch(aout, ich + 1)
    endif
endin


; ─────────────────────────────────────────────────────────────────────────────
; grainsharp — Noisy IDM synth: gritty, granular texture
; ─────────────────────────────────────────────────────────────────────────────
; p4 = amplitude (0–1)
; p5 = frequency (Hz)
; p6 = output channel
;
; Character: FM with a high, rapidly decaying modulation index drives the
; carrier into noise-like territory at the attack. Bandpass-filtered noise
; layers underneath. Result is a rough, gritty grain that decays from chaos
; to pitch — classic IDM sound design.
; ─────────────────────────────────────────────────────────────────────────────
instr grainsharp
    iamp  init p4
    ifreq init p5
    ich   init p6

    ; Sharp amplitude envelope — instant attack, exponential decay
    aenv expseg 0.001, 0.003, 1.0, p3 * 0.18, 0.25, p3 * 0.82, 0.0001

    ; FM voice: modulation index starts very high (noisy) and decays fast
    imod    = ifreq * 5.31           ; inharmonic mod ratio → grainy spectrum
    kmi     linseg 28, p3 * 0.12, 3, p3 * 0.88, 0.4
    amodSig oscili imod * kmi, imod
    afm     oscili 1, ifreq + amodSig

    ; Soft clip the FM for extra grit
    adist = tanh(afm * 6) / tanh(6)

    ; Noise burst: two bandpass layers around low and mid frequencies
    anoise  noise 1, 0.5
    anlo    butterbp anoise, ifreq * 1.8, ifreq * 1.2
    anhi    butterbp anoise, ifreq * 5.5, ifreq * 3.0
    knenv   expseg 0.001, 0.010, 1.0, p3 * 0.25, 0.15, p3 * 0.75, 0.0001

    amix = adist * 0.60 + (anlo + anhi) * 0.35 * knenv
    aout = amix * aenv * iamp
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; metalsharp — Metallic IDM synth: harsh ring, inharmonic
; ─────────────────────────────────────────────────────────────────────────────
; p4 = amplitude (0–1)
; p5 = frequency (Hz)
; p6 = output channel
;
; Character: two FM pairs with inharmonic carrier/modulator ratios (√2 and
; golden ratio). The spectral content is dense and metallic — bell-like at
; the tip, harsh ring throughout. Bright transient that decays slowly.
; ─────────────────────────────────────────────────────────────────────────────
instr metalsharp
    iamp  init p4
    ifreq init p5
    ich   init p6

    ; Envelope: sharp onset, bipartite decay (fast bright, slower tail)
    aenv expseg 0.001, 0.004, 1.0, p3 * 0.08, 0.45, p3 * 0.92, 0.0001

    ; FM pair 1: √2 mod ratio → classic metallic/bell spectrum
    imod1  = ifreq * 1.4142
    kmi1   linseg 22, p3 * 0.06, 3.5, p3 * 0.94, 0.4
    amSig1 oscili imod1 * kmi1, imod1
    afm1   oscili 1.0, ifreq + amSig1

    ; FM pair 2: golden ratio carrier offset → beating inharmonic partials
    icar2  = ifreq * 1.618
    imod2  = ifreq * 2.756
    kmi2   linseg 10, p3 * 0.10, 1.8, p3 * 0.90, 0.2
    amSig2 oscili imod2 * kmi2, imod2
    afm2   oscili 0.45, icar2 + amSig2

    ; High-pass to emphasize the metallic bite above the fundamental
    amix = afm1 * 0.70 + afm2 * 0.30
    amix buthp amix, ifreq * 0.7

    ; Light saturation for edge without obscuring the ring
    amix = tanh(amix * 3.5) / tanh(3.5)

    aout = amix * aenv * iamp
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; purecut — Sine-ish IDM synth: clean, precise, hard transient
; ─────────────────────────────────────────────────────────────────────────────
; p4 = amplitude (0–1)
; p5 = frequency (Hz)
; p6 = output channel
;
; Character: almost pure sine. A tiny band-limited click at onset gives it
; a physical "thump" without adding spectral clutter. The second and third
; harmonic are present but barely audible. Sharp, focused, surgical.
; ─────────────────────────────────────────────────────────────────────────────
instr purecut
    iamp  init p4
    ifreq init p5
    ich   init p6

    ; Near-delta attack — on in 2ms, exponential decay
    aenv expseg 0.001, 0.002, 1.0, p3 - 0.002, 0.0001

    ; Main body: sine + tiny upper harmonics
    asine  oscili 1.00, ifreq, -1
    asine2 oscili 0.07, ifreq * 2, -1   ; 2nd harmonic — warmth
    asine3 oscili 0.02, ifreq * 3, -1   ; 3rd partial — presence
    abody  = asine + asine2 + asine3

    ; Click transient: narrow bandpass noise burst, ~12ms long
    anoise    noise 1, 0
    aclick    butterbp anoise, max:i(ifreq * 3, 900), 500
    aclickenv expseg 0.001, 0.002, 0.7, 0.012, 0.0001, p3 - 0.014, 0.0001
    aclick    = aclick * aclickenv * 0.30

    amix = abody + aclick
    aout = amix * aenv * iamp
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; vaporpad — Vaporwave lush pad: slow attack, thick chorus, deep reverb
; ─────────────────────────────────────────────────────────────────────────────
; p4 = amplitude (0–1)
; p5 = frequency (Hz)
; p6 = output channel (ignored — always outputs stereo to 1+2)
;
; Character: four detuned pulse waves with slow PWM modulation. Lush,
; warm LP filter with moderate resonance. Plate-style reverb (reverbsc)
; drowns the direct signal — 70% wet. Gentle vibrato after attack.
; Always stereo.
; ─────────────────────────────────────────────────────────────────────────────
instr vaporpad
    iamp  init p4
    ifreq init p5

    ; Very slow attack — the signature vaporwave "swell"
    iatt = 1.4
    aenv linseg 0, iatt, 1, p3 - iatt - 0.02, 1, 0.02, 0.0001

    ; Vibrato appears gradually after the attack
    kvibenv  linseg 0, iatt * 0.9, 0, iatt * 0.1, 1, p3 - iatt, 1
    kvibrate lfo 0.0032, 5.5, 0         ; depth in semitones × narrow factor
    kvib     = kvibrate * kvibenv

    ; PWM: slow LFO modulates pulse width 0.35–0.65
    kpwm   lfo 0.045, 0.71, 0
    kwidth = 0.50 + kpwm * 0.15

    ; Four chorus voices — two close, two wider
    ap1 vco2 1.00, ifreq * (1 + kvib) * 0.9988, 2, kwidth
    ap2 vco2 1.00, ifreq * (1 + kvib) * 1.0012, 2, kwidth
    ap3 vco2 0.55, ifreq * (1 + kvib) * 1.0075, 2, kwidth
    ap4 vco2 0.55, ifreq * (1 + kvib) * 0.9925, 2, kwidth
    ; Sub octave pure sine — anchors the bass
    asub oscili 0.50, ifreq * 0.5, -1

    amix = (ap1 + ap2 + ap3 + ap4) * 0.16 + asub * 0.10

    ; LP filter: very slowly opens (starts below 2× fundamental)
    kcutenv linseg ifreq * 1.3, iatt * 2.5, ifreq * 5.5 + 500, p3, ifreq * 5.5 + 500
    afilt moogvcf2 amix, kcutenv, 0.45

    ; Plate reverb — generous tail, moderate brightness cut
    arvL, arvR reverbsc afilt, afilt, 0.86, 7800

    ; Mix: dry/wet 30/70 — very wet for that vapor feel
    aoutL = (afilt * 0.30 + arvL * 0.70) * aenv * iamp
    aoutR = (afilt * 0.30 + arvR * 0.70) * aenv * iamp
    mix_ch aoutL, 1
    mix_ch aoutR, 2
endin


; ─────────────────────────────────────────────────────────────────────────────
; vapordream — Vaporwave chord: retro-piano attack, dreamy sustain
; ─────────────────────────────────────────────────────────────────────────────
; p4 = amplitude (0–1)
; p5 = frequency (Hz)
; p6 = output channel (ignored — always outputs stereo to 1+2)
;
; Character: two saw waves + triangle for a warm attack reminiscent of
; 80s digital pianos, then a heavy low-pass filter closes and a long
; reverb tail carries the sustain. Feels like a Juno chord recorded on
; a dusty cassette. Always stereo.
; ─────────────────────────────────────────────────────────────────────────────
instr vapordream
    iamp  init p4
    ifreq init p5
    ich init p6

    ; Piano-style envelope: moderate attack, then it "softens"
    ; attack portion (0–0.06s): rise
    ; body (0.06–0.5s): slight drop from peak to sustain level
    ; sustain: holds
    aenv linsegr 0, 0.06, 1.0, 0.45, 0.55, p3 - 0.51, 0.55, 3, 0.0001

    ; Body: two slightly detuned saws + triangle for warmth
    asw1 vco2 1.00, ifreq * 0.9994, 0
    asw2 vco2 0.80, ifreq * 1.0006, 0
    atri vco2 0.55, ifreq, 12
    asub oscili 0.30, ifreq * 0.5, -1
    amix = (asw1 + asw2) * 0.30 + atri * 0.16 + asub * 0.10

    ; Remove extreme highs first
    amix butlp amix, 5500

    ; Moving LP: starts at brighter setting, closes toward warm sustain zone
    kcut linseg ifreq * 5 + 400, 0.7, ifreq * 2 + 150, p3 - 0.7, ifreq * 2 + 150
    afilt moogvcf2 amix, kcut, 0.5

    ; Subtle stereo chorus: two LFO-offset copies panned L/R
    kchL lfo 0.003, 2.9, 0
    kchR lfo 0.003, 3.4, 1       ; phase offset for asymmetry
    achL = afilt + oscili:a(0.12, ifreq * (1 + kchL * 0.003), -1)
    achR = afilt + oscili:a(0.12, ifreq * (1 - kchR * 0.003), -1)

    ; Long dream reverb — 88% feedback, warm cutoff
    arvL, arvR reverbsc achL, achR, 0.88, 6500

    ; More dry signal than vaporpad — you hear the attack clearly
    aoutL = (achL * 0.40 + arvL * 0.60) * aenv * iamp
    aoutR = (achR * 0.40 + arvR * 0.60) * aenv * iamp
    mix_ch aoutL, ich
    if(ich == $NCH) then 
      mix_ch aoutR, ich - 1 
    else
      mix_ch aoutR, ich + 1
    endif

endin
