; =============================================================================
; freejazz.orc — Badass electronic voices for the freejazz.idyl solo generator
; #include this file inside the <CsInstruments> section of main.csd
; (after synth.orc, alongside it):   #include "freejazz.orc"
;
; Assumes the following are already defined in main.csd:
;   sr, ksmps, 0dbfs, nchnls
;   opcode mix_ch, 0, ai
;   gamaster[]
;
; Parameter convention (identical to synth.orc):
;   p4 = amplitude (0–1)      p5 = frequency (Hz)      p6 = output channel
;
; These are LEAD / VOICE instruments meant to be machine-gunned by free-jazz
; trigger streams (jazz_density, sheets_rhythm, breathing_rhythm…).  Each note
; reseeds small per-note irregularities (random:i / rspline) so no two notes are
; identical — the inflection of a live, sweating improviser.
;
; Instrument list
; ────────────────────────────────────────────────────────────────────
;  screamlead — overdriven saw lead with a rising vocal-formant SCREAM
;  flangecry  — weeping voice through a jet-engine through-zero flanger
;  glitchvox  — bit-crushed, sample-rate-folded, stutter-gated mangle
;  acidsnarl  — 303-style resonant squelch with a distorted snarl + glide
;  fmcry      — inharmonic FM voice that sobs (pitch dip + swelling vibrato)
;  gritgrowl  — wavefolded low growl, ring-mod AM menace, overwhelming
;  shardbell  — inharmonic FM bell shattered into granular metallic shards
;
; Suggested idyl wrappers (paste into your freejazz patch, with `cs` open):
;   screamlead(spike!, freq, duration=0.3s, amp=f, ch=1) = cs_note(cs, "screamlead", duration, amp, freq, ch)
;   flangecry (spike!, freq, duration=0.6s, amp=f, ch=1) = cs_note(cs, "flangecry",  duration, amp, freq, ch)
;   glitchvox (spike!, freq, duration=0.2s, amp=f, ch=1) = cs_note(cs, "glitchvox",  duration, amp, freq, ch)
;   acidsnarl (spike!, freq, duration=0.3s, amp=f, ch=1) = cs_note(cs, "acidsnarl",  duration, amp, freq, ch)
;   fmcry     (spike!, freq, duration=0.5s, amp=f, ch=1) = cs_note(cs, "fmcry",      duration, amp, freq, ch)
;   gritgrowl (spike!, freq, duration=0.4s, amp=f, ch=1) = cs_note(cs, "gritgrowl",  duration, amp, freq, ch)
;   shardbell (spike!, freq, duration=0.3s, amp=f, ch=1) = cs_note(cs, "shardbell",  duration, amp, freq, ch)
; =============================================================================


; ─────────────────────────────────────────────────────────────────────────────
; screamlead — overdriven lead that screams upward like an altissimo sax
; ─────────────────────────────────────────────────────────────────────────────
; Three tightly-detuned saws + a sub square, slammed through distort1, then a
; high-resonance Moog filter sweep. A resonant bandpass "formant" climbs from
; 2× to 6× the fundamental — the vocal-cord scream. Pitch scoops UP into the
; note and a wavering vibrato (random rate) intensifies over its life.
; ─────────────────────────────────────────────────────────────────────────────
instr screamlead
    iamp  init p4
    ifreq init p5
    ich   init p6

    ; per-note inflection — detuning & vibrato depth differ every note
    idetune = random:i(0.004, 0.013)
    ivibd   = random:i(0.006, 0.022)

    aenv linsegr 0, 0.010, 1, p3 - 0.05, 0.85, 0.04, 0.0001

    ; scoop UP into pitch, then a vibrato that grows + wavers in rate
    kscoop   transeg ifreq * 0.94, 0.05, -4, ifreq, p3 - 0.05, 0, ifreq
    kvibrate rspline 4.5, 7.8, 0.3, 1.3
    kvibenv  linseg 0, p3 * 0.25, 1, p3 * 0.75, 1
    kvib     oscili ivibd * kvibenv, kvibrate
    kf = kscoop * (1 + kvib)

    ; source: detuned saw triad + sub square
    a1  vco2 0.6, kf * (1 - idetune)
    a2  vco2 0.6, kf
    a3  vco2 0.6, kf * (1 + idetune)
    asq vco2 0.4, kf * 0.5, 2, 0.5
    asrc = (a1 + a2 + a3) * 0.30 + asq * 0.20

    ; SCREAM formant: resonant bandpass climbing 2×→6×→3× the fundamental
    kform  transeg ifreq * 2, 0.2, 2, ifreq * 6, p3 - 0.2, -2, ifreq * 3
    ascrm  butterbp asrc, kform, kform * 0.3
    asrc = asrc * 0.5 + ascrm * 0.9

    ; heavy asymmetric overdrive
    adist distort1 asrc, 4, 0.6, 0.0, 0.9

    ; resonant Moog sweep — squeals near the top
    kcut  transeg ifreq * 3, 0.15, 2, ifreq * 9, p3 - 0.15, -3, ifreq * 4
    afilt moogvcf2 adist, kcut, 0.82

    aout = limit(afilt * aenv * iamp * 1.2, -1, 1)
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; flangecry — a crying voice swept by a deep through-zero-ish flanger
; ─────────────────────────────────────────────────────────────────────────────
; Two detuned saws + triangle, softly saturated, then a flanger with a slow
; LFO-modulated delay and high feedback (jet whoosh). The pitch FALLS into the
; note and sobs; the vibrato wavers at a random rate. Weepy, vocal, drifting.
; ─────────────────────────────────────────────────────────────────────────────
instr flangecry
    iamp  init p4
    ifreq init p5
    ich   init p6

    ivib = random:i(0.010, 0.030)

    aenv linsegr 0, 0.03, 1, p3 - 0.10, 0.8, 0.07, 0.0001

    ; cry: fall from slightly sharp into pitch, then waver
    kfall transeg ifreq * 1.04, 0.08, -3, ifreq, p3 - 0.08, 0, ifreq
    kvr   rspline 3, 6, 0.2, 0.9
    kve   linseg 0, p3 * 0.3, 1, p3 * 0.7, 1
    kvib  oscili ivib * kve, kvr
    kf = kfall * (1 + kvib)

    a1   vco2 0.7, kf * 0.999
    a2   vco2 0.7, kf * 1.001
    atri vco2 0.4, kf, 12
    asrc = (a1 + a2) * 0.40 + atri * 0.30

    asrc = tanh(asrc * 2) / tanh(2)
    asrc moogvcf2 asrc, ifreq * 5 + 600, 0.30

    ; flanger — slow LFO drives a 0.6–2.8 ms delay, 0.85 feedback
    kflr  rspline 0.15, 0.55, 0.05, 0.25
    klfo  oscili 1, kflr
    adel = a(0.0006 + 0.0022 * (klfo * 0.5 + 0.5))
    aflg  flanger asrc, adel, 0.85

    amix = asrc * 0.4 + aflg * 0.7
    aout = limit(amix * aenv * iamp * 1.4, -1, 1)
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; glitchvox — barely-recognizable melody: bit-crush + SR-fold + stutter gate
; ─────────────────────────────────────────────────────────────────────────────
; A clear pitched core (with the odd random octave/5th jump) is destroyed by
; (1) a ~3–6 bit quantiser, (2) the `fold` opcode at a randomly-wandering rate
; (aliasing screech), and (3) a random-rate square-wave gate that chops the
; amplitude into stutters. The tune survives only as a rumour.
; ─────────────────────────────────────────────────────────────────────────────
instr glitchvox
    iamp  init p4
    ifreq init p5
    ich   init p6

    aenv linsegr 0, 0.005, 1, p3 - 0.02, 0.9, 0.015, 0.0001

    ; melodic core — occasional octave / fifth displacement
    ijump = random:i(0, 1)
    imult = (ijump > 0.80 ? 1.5 : (ijump < 0.12 ? 2 : 1))
    asrc  vco2 0.7, ifreq * imult
    asrc = asrc + oscili:a(0.3, ifreq * imult * 2)

    ; bit-crush (3–6 bits)
    ibits  = random:i(3, 6)
    isteps = 2 ^ ibits
    acr = int(asrc * isteps) / isteps

    ; sample-rate reduction — random hold length → aliasing glitch
    kfold rspline 1, 40, 0.4, 6
    afold fold acr, kfold

    ; stutter gate — random-rate, random-duty pulse chops amplitude
    krate rspline 6, 32, 0.3, 2
    kduty rspline 0.2, 0.8, 0.5, 3
    kph   phasor krate
    kgate = (kph < kduty ? 1 : 0)
    kgate port kgate, 0.0008

    amix = afold * kgate
    amix = amix * 0.7 + butterbp(amix, ifreq * 3, ifreq * 2) * 0.4

    aout = limit(amix * aenv * iamp, -1, 1)
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; acidsnarl — TB-303-ish resonant squelch with bite, glide and overdrive
; ─────────────────────────────────────────────────────────────────────────────
; Saw + narrow pulse glide into the note (303 slide). A high-resonance Moog
; cutoff envelope squelches from low → accent-scaled peak → low, then distort1
; adds the snarl. Per-note random accent varies cutoff peak and drive.
; ─────────────────────────────────────────────────────────────────────────────
instr acidsnarl
    iamp  init p4
    ifreq init p5
    ich   init p6

    iaccent = random:i(0.5, 1.0)

    aenv linsegr 0, 0.006, 1, p3 - 0.04, 0.5, 0.03, 0.0001

    ; glide up from half-pitch (303 portamento)
    kf  port ifreq, 0.04, ifreq * 0.5
    asaw vco2 0.8, kf
    asq  vco2 0.5, kf, 2, 0.45
    asrc = asaw * 0.6 + asq * 0.4

    ; squelchy resonant sweep, peak scaled by accent
    kcut  transeg ifreq * 1.5, 0.02, 2, ifreq * 8 * iaccent, p3 - 0.02, -4, ifreq * 1.2
    afilt moogvcf2 asrc, kcut, 0.84

    adist distort1 afilt, 3 * iaccent, 0.7, 0.0, 0.85
    aout = limit(adist * aenv * iamp * 0.8, -1, 1)
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; fmcry — inharmonic FM voice that sobs: pitch dip + swelling wavery vibrato
; ─────────────────────────────────────────────────────────────────────────────
; A √2 (metallic) modulator on the carrier, plus a slightly-detuned second FM
; voice for beating. The pitch dips ~3% mid-note (the sob) and a vibrato swells
; in at a random rate. High-passed and softly clipped — vocal, aching, alive.
; ─────────────────────────────────────────────────────────────────────────────
instr fmcry
    iamp  init p4
    ifreq init p5
    ich   init p6

    ivib = random:i(0.008, 0.025)

    aenv linsegr 0, 0.02, 1, p3 - 0.12, 0.7, 0.10, 0.0001

    kvr  rspline 4, 7, 0.3, 1.5
    kve  linseg 0, p3 * 0.35, 1, p3 * 0.65, 0.8
    kvib oscili ivib * kve, kvr
    ; sob: dip then recover
    ksob transeg ifreq, 0.10, 2, ifreq * 0.97, 0.15, 2, ifreq
    kf = ksob * (1 + kvib)

    ; FM index decays from bright onset to a sustained shimmer
    kmi   linseg 6, p3 * 0.2, 2.2, p3 * 0.8, 0.8
    kmodf = kf * 1.4142
    amod  oscili kmodf * kmi, kmodf
    acar  oscili 1.0, a(kf) + amod

    ; detuned beating partner
    kmodf2 = kf * 1.4142 * 1.005
    amod2  oscili kmodf2 * kmi, kmodf2
    acar2  oscili 0.8, a(kf) * 1.003 + amod2

    asrc = acar * 0.6 + acar2 * 0.5
    asrc buthp asrc, ifreq * 0.8
    asrc = tanh(asrc * 2.2) / tanh(2.2)

    aout = limit(asrc * aenv * iamp, -1, 1)
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; gritgrowl — wavefolded low growl with ring-mod AM menace
; ─────────────────────────────────────────────────────────────────────────────
; Saw + sub sine pushed through a sine wavefolder (foldback increases over the
; note) for dense buzzing harmonics, then amplitude-modulated by a 25–70 Hz
; LFO (random, wavering rate) for the growl. Low resonant filter + tanh clip.
; Sits low and overwhelms. Best in the bottom octave or two.
; ─────────────────────────────────────────────────────────────────────────────
instr gritgrowl
    iamp  init p4
    ifreq init p5
    ich   init p6

    aenv linsegr 0, 0.008, 1, p3 - 0.06, 0.85, 0.05, 0.0001

    asaw vco2 0.7, ifreq
    asub oscili 0.6, ifreq * 0.5, -1
    asrc = asaw * 0.6 + asub * 0.5

    ; wavefolding — foldback depth grows then eases
    kfold linseg 1.5, p3 * 0.3, 3.5, p3 * 0.7, 2
    afold = sin(asrc * kfold * 3.14159)

    ; growl: low-freq AM at a wavering random rate
    kgr  rspline 25, 70, 0.4, 3
    kam  oscili 0.5, kgr
    agrowl = afold * (0.6 + kam)

    afilt moogvcf2 agrowl, ifreq * 4 + 200, 0.6
    adist = tanh(afilt * 3) / tanh(3)
    aout = limit(adist * aenv * iamp, -1, 1)
    mix_ch aout, ich
endin


; ─────────────────────────────────────────────────────────────────────────────
; shardbell — inharmonic FM bell shattered into granular metallic shards
; ─────────────────────────────────────────────────────────────────────────────
; A random-ratio FM bell, ring-modulated by another inharmonic partial, then a
; very fast random-rate gate fractures it into glassy shards. High-passed and
; clipped for brittle bite. Bright, scattered, barely a melody — pure texture.
; ─────────────────────────────────────────────────────────────────────────────
instr shardbell
    iamp  init p4
    ifreq init p5
    ich   init p6

    iratio = random:i(1.3, 2.8)

    aenv expseg 0.001, 0.004, 1, p3 - 0.004, 0.0001

    ; inharmonic FM bell
    kmi  expseg 8, 0.05, 2, p3, 0.3
    amod oscili ifreq * iratio * kmi, ifreq * iratio
    acar oscili 1, ifreq + amod

    ; ring-mod with another inharmonic partial → metallic clang
    arm  = acar * oscili:a(1, ifreq * iratio * 1.5)
    asrc = acar * 0.6 + arm * 0.5

    ; granular scatter — fast random gate breaks it into shards
    krate rspline 20, 80, 0.5, 5
    kph   phasor krate
    kgate = (kph < 0.3 ? 1 : 0)
    kgate port kgate, 0.0005

    amix = asrc * kgate
    amix buthp amix, ifreq * 1.2
    amix = tanh(amix * 2.5) / tanh(2.5)

    aout = limit(amix * aenv * iamp * 0.9, -1, 1)
    mix_ch aout, ich
endin
