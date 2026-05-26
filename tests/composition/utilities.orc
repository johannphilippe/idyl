; implementation of live-coding kill utility
instr KillImpl
  Sinstr = p4
  if (nstrnum(Sinstr) > 0) then
    turnoff2(Sinstr, 0, 0)
  endif
  turnoff
endin

; live-coding killer 
opcode kill, 0, S
	Sinstr xin
	schedule("KillImpl", 0, .05, Sinstr)
endop

instr StopImpl
  Sinstr = p4
  if (nstrnum(Sinstr) > 0) then
    turnoff2(Sinstr, 0, 1)
  endif
  turnoff
endin
opcode stop, 0, S
  Sinstr xin 
	schedule("StopImpl", 0, .05, Sinstr)
endop
    
; live-coding starter
opcode start, 0, S
	Sinstr xin
	if (nstrnum(Sinstr) > 0) then
    	    kill(Sinstr)
    	    schedule(Sinstr, ksmps / sr, -1)
	endif
endop

; util to read file as string 
opcode read_file, S, S
    Spath xin

    Sstr = ""
    ivalid filevalid Spath
    if(ivalid == 0 ) then 
        prints("Errr - read_file UDO - Path to file is invalid -> Exiting \n")
        igoto noread
    endif
    read:
        Sline, iLineNum readfi Spath
        Sstr strcat Sstr, Sline
        if(iLineNum != -1) igoto read   
    noread:
    xout Sstr
endop

/*
        Opcodes for phasor & tempo sync 
*/
opcode trunc, k, k
  kx xin
  ky = (kx < 0) ? -(floor(-kx)) : floor(kx)
  xout ky
endop
opcode mfmod, k, kk
  kone, ktwo xin
  kres = kone - trunc(kone/ktwo) * ktwo
  xout kres
endop

opcode phase_trig, k, k
  kphase xin
  kold init 1
  ktrigger = 0
  if(kold > kphase) then 
    ktrigger = 1
  endif
  kold = kphase
  xout ktrigger
endop

opcode downprint, 0, kki
  kprint, kdown, imargin xin
  kcnt init 0
  if(kcnt == 0) then 
    printk2(kprint, imargin)
  endif
  kcnt = (kcnt + 1) % kdown
endop


opcode filter_first_trig, k, k
  ktrig xin
  ksub init 1
  ktrig -= ksub
  ksub = 0
  xout ktrig
endop

opcode check_trig, k, k
  ktrig xin 
  kf = filter_first_trig(ktrig)
  kres = (changed:k(kf) > 0 && kf > 0) ? 1 : 0
  xout kres
endop

opcode to_tempo_dur, k, k
  kdur xin
  ktdur = kdur / gktempo
  xout ktdur
endop

  
opcode phasor_fqmult, k, kk
  kphase, kmult xin
  if(kmult <= 0) then 
    kmult = 1
  endif
  kfphase init 0
  if(kmult <= 1) then 
    kfphase = mfmod(gkslow * (kmult * 64), 1)
  elseif(kmult > 1) then 
    kfphase = mfmod(gkslow * (kmult * 64), 1)
    ;kfphase= mfmod(gkphasor * kmult, 1)
  endif
  xout kfphase
endop

opcode rh_subd, k, k[]P
  karr[], kpdiv xin
  ilen = lenarray(karr)
  if(kpdiv <= 0) then 
    kpdiv = ilen
  endif
  kdiv = kpdiv
  kph = phasor_fqmult(gkphasor, kdiv )
  ktr = phase_trig(kph)
  kidx init 0
  ktrigger = 0
  if(ktr == 1) then 
    ktrigger = karr[kidx]
    kidx = (kidx+1) % ilen
  endif
  xout ktrigger
endop

opcode rhythm_sub, k, k[]P
  karr[], kpdiv xin
  ilen = lenarray(karr)
  if(kpdiv <= 0) then 
    kpdiv = ilen
  endif
  kdiv = kpdiv
  kph = phasor_fqmult(gkphasor, kdiv)
  ktr = phase_trig(kph)
  kidx init 0
  ktrigger = 0
  if(ktr == 1) then 
    ktrigger = karr[kidx]
    kidx = (kidx+1) % ilen
  endif
  xout ktrigger
endop




// Segment helpers 
opcode lin, k, iii
    ifrom, ito, idur xin 
    kres = linseg:k(ifrom, idur, ito)
    xout kres
endop
opcode lin1, k, ii 
    ifrom, ito xin 
    kres = linseg:k(ifrom, 1, ito)
    xout kres
endop
opcode lin5, k, ii 
    ifrom, ito xin 
    kres = linseg:k(ifrom, 5, ito)
    xout kres
endop
opcode lin10, k, ii 
    ifrom, ito xin 
    kres = linseg:k(ifrom, 10, ito)
    xout kres
endop
opcode lin15, k, ii 
    ifrom, ito xin 
    kres = linseg:k(ifrom, 15, ito)
    xout kres
endop


// Random integers
opcode rint, i, ii
	imin, imax xin
	irnd = int(random:i(imin, int(imax) + 0.99))
	xout irnd
endop

opcode rint, k, kk
	kmin, kmax xin
	krnd = int(random:k(kmin, int(kmax) + 0.99))
	xout krnd
endop

opcode rnd_alt, k, kk
	kone, ktwo xin 
	krnd = random:k(0, 1)
	kres = kone 
	if(krnd > 0.5) then 
		kres = ktwo
	endif
	xout kres
endop

opcode random_array, i[], iii
	isize, imin, imax xin
	iArr[] init isize
	icnt init 0
    while icnt < isize do 
        iArr[icnt] = random:i(imin, imax)  
        icnt += 1
    od
	xout iArr
endop

// Random array of int
opcode random_array_int, i[], iii
	isize, imin, imax xin
	iArr[] init isize
	icnt init 0
    while icnt < isize do 
        iArr[icnt] = rint:i(imin, imax)  
        icnt += 1
    od
	xout iArr
endop


// Random Array of int without duplicates
opcode random_array_int_nodup, i[], iii
	isize, imin, imax xin
	iArr[] init isize
	icnt init 0
    while icnt < isize do 
        restart:
        iArr[icnt] = rint:i(imin, imax)  
        icmp = icnt
        while icmp >= 0 do 
            if(iArr[icnt] == iArr[icmp]) igoto restart
            icmp -= 1
        od
        icnt += 1
    od
	xout iArr
endop

opcode pick, k, k[]
	karr[] xin 
	ilen lenarray karr
	krnd = rint:k(0, ilen-1)
	kres = karr[krnd]
	xout kres 
endop

// Zero filtering in audio
opcode filter_zero, a, a
        asig xin
        kcnt = 0
        while kcnt < ksmps do
                if(asig[kcnt] == 0) then
                        ksign = asig[kcnt] > 0 ? 1 : -1
                        asig[kcnt] = 0.0001 * ksign
                endif
                kcnt += 1
        od

        xout asig
endop

// Coordinates 

// Cartesian 3D distance 
opcode dist_3D, i, iiiiii
        ix1,iy1,iz1,ix2,iy2,iz2 xin
        ires = sqrt( pow(ix2 - ix1, 2) + pow(iy2 - iy1, 2) + pow(iz2 - iz1, 2)  )
        xout ires
endop

opcode dist_3D, k, kkkkkk
        kx1,ky1,kz1,kx2,ky2,kz2 xin
        kres = sqrt( pow(kx2 - kx1, 2) + pow(ky2 - ky1, 2) + pow(kz2 - kz1, 2)  )
        xout kres
endop

opcode dist_3D, a, aaaaaa
        ax1,ay1,az1,ax2,ay2,az2 xin
        ares = sqrt( pow(ax2 - ax1, 2) + pow(ay2 - ay1, 2) + pow(az2 - az1, 2)  )
        xout ares
endop


// Cartesian to spherical and invert
opcode xyz_to_aed, aaa,aaa
        ax, ay, az xin
        aRad = sqrt( (ax * ax) + (ay * ay) + (az * az) )
        // Careful, must not be 0 (0 distance is veryyyy close)
        aRad = filter_zero(aRad)

        aInc = cosinv( az / aRad)
        aElev = ($M_PI / 2)  - aInc
        aaZ = taninv2(ay, ax)
        xout  aaZ, aElev, aRad
endop

opcode aed_to_xyz, aaa, aaa
        aa,ae,ad xin
        ; Warning here, division by 0
        az =  sqrt( pow(ad, 2) * pow(cos(aa), 2) / (1 + pow(cos(aa), 2) * pow(tan(ae), 2)))
        ax = az * tan(aa)
        ay = az * tan(ae)
        xout ax, ay, az
endop

opcode xyz_to_aed, kkk, kkk
        kx, ky, kz xin
        kRad = sqrt( (kx * kx) + (ky * ky) + (kz * kz) )
        // Careful, must not be 0 (0 distance is veryyyy close)
        if(kRad == 0) then
                ksign = (kRad > 0) ? 1 : -1
                kRad = 0.00001  * ksign
        endif
        kInc = cosinv( kz / kRad)
        kElev = ($M_PI / 2)  - kInc
        kaZ = taninv2(ky, kx)
        xout  kaZ, kElev, kRad
endop

opcode aed_to_xyz, kkk, kkk
        ka,ke,kd xin
        ; Warning here, division by 0
        kz =  sqrt( pow(kd, 2) * pow(cos(ka), 2) / (1 + pow(cos(ka), 2) * pow(tan(ke), 2)))
        kx = kz * tan(ka)
        ky = kz * tan(ke)
        xout kx, ky, kz
endop


; works with triggers 
opcode hold_smps, k, kk
	kval, kn_smps xin

	kto_hold init 0
	kcnt init 0
	ktrig = trigger(kval, 0.5, 0)
	if(ktrig > 0) then 
		kcnt = kn_smps
		kto_hold = kval
	endif

	
	kcnt -= ksmps
	kcnt = limit:k(kcnt, 0, kn_smps)
	kres init 0
	kres = kval
	if(kcnt > 0) then 
		kres = kto_hold
	endif

	xout kres
endop

opcode hold, k, kk
	kval, ktim xin
	kres = hold_smps(kval, ktim * sr)
	xout kres
endop	


// interpolator
opcode linitp, k, ki
	kdest, itime xin
	ktrig = changed:k(kdest)
	kold init i(kdest)
	klast init i(kdest)
	if(ktrig == 1) then 
		kold = klast
	endif
	kseg = triglinseg(ktrig, 0, itime, 1)
	kres =	kold + (kseg * (kdest - kold)) 

	klast = kdest
	xout kres
endop	

; segment between 0 andd 1 when triggered
opcode lseg, k, kk
	kdur, ktrig xin
	kdur_smps init 0
	kres init 0
	kcnt init 0

	kdur_smps = kdur * sr
	krun init 1
	if(ktrig == 1 && krun == 0) then 
		kcnt = 0
		kres = 0
		krun = 1
	endif
	if(kcnt < kdur_smps) then 
		kcnt_norm = kcnt / kdur_smps
		kres = kcnt_norm
		kcnt += ksmps
	else
		krun = 0
		kres = 0
	endif
	xout kres
endop

opcode lseg, a, kk
	kdur, ktrig xin
	ares = interp(lseg:k(kdur, ktrig))
	xout ares
endop

; Simple triggered ramp from 0 to 1
opcode trigline, k, ki
        ktrig, idur xin 
        ktrig = trigger(ktrig, 0.5, 0)
        klin = triglinseg:k(ktrig, 0, idur, 1)
        xout klin
endop

opcode trigline, a, ki
        ktrig, idur xin 
        ktrig = trigger(ktrig, 0.5, 0)
        alin = triglinseg:a(ktrig, 0, idur, 1)
        xout alin
endop

; Funky random modulator generator
opcode funkymod, k, kkk
	kamp, kfq, kduty xin

	; 0 sine
	; 1 tri
	; 2 square bipolar
	; 2 square unipolar
	; 4 saw up
	; 5 saw down
	; 6 pwm

  iwaves[] = fillarray(0, 1, 4, 5)
  iwave = iwaves[rint:i(0, lenarray(iwaves)-1)]
  print iwave
  klfo = limit( lfo(kamp * 0.5, kfq , iwave) + (kamp * 0.5), 0, 1)

/*
	iwave = rint:i(0, 6)
  print iwave

	ilf_amp = (iwave < 6) ? 1 : 0
	ipwm_amp = 1 - ilf_amp

  ilfowave = iwave
  if(iwave == 6) then 
    ilfowave = 1
  endif
  if(ilfowave == 3 || ilfowave == 2) then 
    ilfowave = 1
  endif

	klfo = lfo:k(kamp, kfq, ilfowave)
	if(iwave < 3) then 
		klfo += kamp
		klfo /= 2
	endif

	klfo *= ilf_amp
	
	kpwm = vco2(kamp*0.5, kfq, 2, kduty) + (kamp * 0.5)
	;kpwm *= ipwm_amp

	xout klfo + kpwm
*/
  xout klfo
endop

; Modulator based on hypercurves
opcode hypermod, k, kki[]
	kamp, kfq, iarr[] xin
	iwaveidx = rint:i(0, lenarray(iarr)-1)
	iwave = iarr[iwaveidx]
	kval = tablei:k(phasor:k(kfq), iwave, 1) * kamp
	xout kval
endop

opcode sf, a, Spo
	Sfile, iloop, istart xin
	asigs[] diskin2 Sfile, 1, istart, iloop
	ares = sumarray(asigs) / lenarray(asigs)
	xout ares
endop

opcode gate, k, kk
	ktrig, kdur xin
	kretrig = ktrig + vdel_k(ktrig, kdur, 1)
	khold = samphold(ktrig, kretrig)
	xout khold
endop

; routing utility
opcode rt,0,iiP 
	ifrom, ito, kgain xin 
	ain = inch(ifrom) * kgain
	mix_ch(ain, ito)
endop
opcode rt,0,kkP 
	kfrom, kto, kgain xin 
	ain = inch(kfrom) * kgain
	mix_ch(ain, kto)
endop

opcode incr, k, kkk
	ktrig, kmin, kmax xin 
	kcnt init 0
	if(ktrig == 1) then 
		kcnt = (kcnt + 1) % kmax
	endif
	kres = kcnt + kmin
	xout kres
endop

; printing utility 
opcode print_str_array, 0, S[]
	Sarr[] xin 
	ilen lenarray Sarr
	icnt init 0
	while icnt < ilen do 
		printf_i("--> At %d : %s \n",1,  icnt+1, Sarr[icnt])
		icnt += 1
	od
endop

opcode chance, k, kk
  kmin, kmax xin
  kres = random:k(kmin, kmax)
  xout kres
endop

opcode alternate, k, kkkk
  kone, ktwo, kspeed, kduty xin

  if(kspeed <= 0) then 
    kspeed = 1
  endif
  
  kph = phasor_fqmult(gkphasor, kspeed)
  kres init i(kone)
  if(kph < kduty) then 
    kres = kone
  else 
    kres = ktwo
  endif
  xout kres
endop

opcode trajectory, k, KKK
  kfrom, kdur, kto xin
  kdur = to_tempo_dur(kdur)
  kres = linseg:k( i(kfrom), i(kdur), i(kto) )
  xout kres
endop

opcode oscillation, k, kkk
  kmin, kmax, kspeed xin
  if(kmin > kmax) then
    ktmp = kmin
    kmin = kmax
    kmax = ktmp
  endif
  kspeed = limit:k(kspeed, 0.001, 1000)
  
  kfq = gktempo * kspeed
  iphs init i(gkphasor)
  kosc = oscili(0.5, kfq, -1, iphs) + 0.5
  kres = kosc * (kmax - kmin) + kmin
  xout kres
endop

opcode sequence, k, k[]k
	kvals[], ktrig xin
	ilen lenarray kvals
	kindex init 0
	if(trigger:k(ktrig, 0.5, 0) == 1) then 
		kindex = (kindex + 1) % ilen 
	endif
	kres = kvals[kindex]
	xout kres
endop

opcode cnv, a, aiPo
  ain, iir, kmix, isize xin 
  kmix = limit:k(kmix, 0, 1)

  if(isize == 0) then 
    isize = 2048
  endif
  asig ftconv ain, iir, isize
  ares = (asig * kmix) + (ain * (1 - kmix))
  
  xout ares
endop