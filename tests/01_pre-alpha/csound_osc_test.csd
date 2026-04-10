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

; Test OSC scheduling 
instr 2 
    print p3
    print p4
    print p5 
    ao = oscili(p4, p5) * expseg(1, p3, 0.01) * 0.3
	outs, ao, ao
endin

instr 3 ; scheduler 
	kfreq init 0
	kk OSClisten gihandle, "/csound/scheduler", "f", kfreq

	if(kk == 1) then 
		schedulek(2, 0, 1, 0.3, kfreq)
	endif

endin

</CsInstruments>
; ==============================================
<CsScore>
f 0 z
i 1 0 -1 
i 3 0 -1


</CsScore>
</CsoundSynthesizer>

