<CsoundSynthesizer>
<CsOptions>
-odac 
-+rtaudio=jack
</CsOptions>
; ==============================================
<CsInstruments>

sr	=	48000
ksmps	=	32
nchnls	=	8
nchnls_i =  	8
0dbfs	=	1

instr 1	

	ich init p6
	iamp init p4
	ifq init p5
	print ich 
	print iamp 
	print ifq 
	aenv = expseg:a(0.001, p3/2, 1, p3/2, 0.001)
	asig = vco2(iamp, ifq) * aenv * 0.3

	outch(ich, asig)
endin

</CsInstruments>
; ==============================================
<CsScore>

f 0 z

</CsScore>
</CsoundSynthesizer>

