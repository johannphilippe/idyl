<CsoundSynthesizer>
<CsOptions>
-odac:null
-iadc:null
-+rtaudio=jack
</CsOptions>
; ==============================================
<CsInstruments>

sr	=	48000
ksmps	=	32
nchnls	=	24
nchnls_i= 	10
0dbfs	=	1

gioffset = 16

gkgain init 0.5
gamaster[] init 2 

opcode mix_ch, 0, ai
	asig, ich xin 

	gamaster[ich-1] = gamaster[ich-1] + asig
endop

; ─── Instrument 1 : Deep Bass Kick ────────────────────────────────────────────
; p4 = amplitude (e.g. 0.9)
; p5 = base (peak) frequency in Hz (e.g. 180)
; p6 = target (floor) frequency in Hz (e.g. 40)
; p7 = pitch decay time in seconds (e.g. 0.06)
; Suggested score line: i 1  0  0.5  0.9  180  40  0.06
instr deepkick

  ; ── Amplitude envelope ────────────────────────────────────────────────────────
  ; Very sharp attack (0.002 s), long exponential decay
  iAmp       =        p4
  aEnv       expseg  0.001, 0.002, iAmp, p3 - 0.002, 0.0001

  ; ── Pitch envelope ────────────────────────────────────────────────────────────
  ; Exponential sweep from p5 (high) down to p6 (low) over p7 seconds,
  ; then holds at p6 for the tail.
  iFreqHi    =        p5
  iFreqLo    =        p6
  iPitchDec  =        p7
  kFreq      expseg  iFreqHi, iPitchDec, iFreqLo, p3 - iPitchDec, iFreqLo

  ; ── Sine oscillator (body) ────────────────────────────────────────────────────
  aSine      oscili   1, kFreq, -1       ; ftable 1 = sine (GEN10)

  ; ── Noise transient (click / attack bite) ─────────────────────────────────────
  ; Short burst of white noise shaped by a very fast envelope,
  ; band-passed around 3 kHz to give that sharp mid snap.
  aNoise     noise    1, 0.5
  aNBP       butterbp aNoise, 3000, 2000   ; centred at 3 kHz, wide band
  aClickEnv  expseg  0.001, 0.001, 0.5, 0.015, 0.0001, p3 - 0.016, 0.0001
  aClick     =        aNBP * aClickEnv

  ; ── Mix and output ────────────────────────────────────────────────────────────
  aOut       =        (aSine * aEnv) + (aClick * 0.35)
  mix_ch aOut, 1
  mix_ch aOut, 2


endin


; ─── Instrument 2 : Dirty Kick ────────────────────────────────────────────────
; p4 = amplitude (e.g. 0.85)
; p5 = peak frequency in Hz (e.g. 120)
; p6 = floor frequency in Hz (e.g. 35)
; p7 = drive amount — waveshaping gain (e.g. 8 ; more = nastier)
; Suggested score line: i 2  0  0.5  0.85  120  35  8
instr dirtykick

  ; ── Amplitude envelope ────────────────────────────────────────────────────────
  iAmp       =        p4
  iDrive    =        p7
  aEnv       expseg  0.001, 0.003, iAmp, p3 - 0.003, 0.0001

  ; ── Pitch envelope (slower sweep = fatter, dirtier feel) ──────────────────────
  kFreq      expseg  p5, 0.1, p6, p3 - 0.1, p6

  ; ── Main oscillator with heavy waveshaping ────────────────────────────────────
  ; Drive the sine hard then normalise back — produces odd harmonics / grit
  aSine      oscili   1, kFreq, -1
  aDriven    =        tanh(aSine * iDrive) / tanh(iDrive)   ; normalised soft clip

  ; ── Sub layer (one octave down, pure sine — adds fundamental mud) ─────────────
  aSub       oscili   0.5, kFreq * 0.5, -1
  aSubEnv    expseg  0.001, 0.005, 0.5, p3 - 0.005, 0.0001
  aSubOut    =        aSub * aSubEnv

  ; ── Dirty noise layer (the "filth") ───────────────────────────────────────────
  ; Pink-ish noise → bandpass around 200–600 Hz → slow decay = vintage dirt
  aNoise     pinkish  1                              ; pink-ish noise opcode
  aNBP       butterbp aNoise, 350, 400              ; dirty mid-low band
  aNoise2    butterbp aNoise, 80, 60               ; another band: sub rumble
  aNoiseEnv  expseg  0.001, 0.004, 0.8, 0.08, 0.2, p3 - 0.084, 0.0001
  aDirt      =        (aNBP + aNoise2) * aNoiseEnv

  ; ── Bit-crush effect (quantise to fake low bit depth) ─────────────────────────
  ; Reduces to ~6 bits of resolution on the oscillator signal — lo-fi crunch
  iBits      =        6
  iSteps     =        2 ^ iBits
  aCrushed   =        int(aDriven * iSteps) / iSteps

  ; ── Final mix ─────────────────────────────────────────────────────────────────
  ; Crushed oscillator + sub + dirt, shaped by global amplitude envelope
  aMix       =        (aCrushed * 0.55) + (aSubOut * 0.25) + (aDirt * 0.20)
  aOut       =        aMix * aEnv

  ; ── Output stage soft clipper (keeps it loud but not over 0 dBFS) ─────────────
  aOut       =        tanh(aOut * 1.4) / tanh(1.4)
  mix_ch aOut, 1
  mix_ch aOut, 2

endin

instr master 
	asigL = gamaster[0] * gkgain 
	asigR = gamaster[1] * gkgain

	gamaster[0] = 0
	gamaster[1] = 0

	outch(1+gioffset, asigL)
	outch(2+gioffset, asigR)
endin


instr sine
	iamp init p4
	inote init p5 
	ich init p6 
	print(p5)

	ao = vco2(iamp, mtof:i(inote) ) * expseg:a(1, p3, 0.001)
	gamaster[ich - 1] = gamaster[ich - 1] + ao

endin

instr 888 ; direct output 
	print(p4)
	print(p5)
	ich init p4 
	ioch init p5 + gioffset 
	icheck init p5 
	ain = inch(ich) * 0.4
	kin = k(ain)
	outch(ioch, ain)
	outch(icheck, ain)
endin

instr dusk
	prints(">>>>>> dusk started\n")
	print(p4)
	turnoff2_i(p4, 0, 1)
	turnoff
endin

</CsInstruments>
; ==============================================
<CsScore>
f 0 z

i "master" 0 -1


</CsScore>
</CsoundSynthesizer>

