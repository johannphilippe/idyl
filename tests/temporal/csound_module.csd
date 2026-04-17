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

instr first
    print p3
    print p4
    print p5 
    ao = vco2(p4, p5) * expseg(1, p3, 0.01) * 0.3
    ipan random 0, 1
    al, ar pan2 ao, ipan
	outs, al, ar
endin

</CsInstruments>
; ==============================================
<CsScore>
f 0 z

</CsScore>
</CsoundSynthesizer>

