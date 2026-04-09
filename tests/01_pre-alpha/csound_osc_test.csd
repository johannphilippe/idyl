<CsoundSynthesizer>
<CsOptions>
-odac
</CsOptions>
; ==============================================
<CsInstruments>

sr	=	48000
ksmps	=	32
nchnls	=	2
0dbfs	=	1

gihandle OSCinit 9000
instr 1	
	kfreq init 0
	kk  OSClisten gihandle, "/csound/freq", "f", kfreq
	kfreq = limit:k(kfreq, 20, 5000)
	ao = oscili(0.2, kfreq) 

	outs, ao, ao

endin

</CsInstruments>
; ==============================================
<CsScore>
f 0 z
i 1 0 -1 


</CsScore>
</CsoundSynthesizer>

