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



instr direct 
	ain = inch(1)  * 0.4
	kin = k(ain) 
	printk2(kin)
	outch(17, ain)
endin

</CsInstruments>
; ==============================================
<CsScore>
f 0 z

i "master" 0 -1

i "direct" 0 -1


</CsScore>
</CsoundSynthesizer>

