	cblock
	madr,mscr
	hscr,lscr
	vscr,wscr,xscr,pscr
	qscr,rscr,sscr,psig
	qsig,hstl,dstl,ustl
	vstl
;vars
	abfo
	aslu:2,aslh
	awsc:2,ahsc:2,ausc,atsc
	ausg
	endc

#ifdef M427	;D34-W22-H7
Adata macro
	org	0x2100+48
	de	1,80,40,0	;1-2
	de	150,0,255,0	;ovverrides B2pi,drop,xsig,tstl
	de	100,130,20,0	;drop(2),sw
	de	0,0,0,0
	de	0,0,0,0
	de	0,0,0,0
	org	0x2100+72
	de	3,80,60,0	;3-4
	de	150,0,255,13	;ovverrides B2pi,drop,xsig,tstl
	de	20,125,40,0	;drop(2),sw
	de	0,0,0,0
	de	0,0,0,0
	de	0,0,0,0
	org	0x2100+96
	de	4,100,100,0	;F-mode
	de	100,0,255,0
	de	80,0,5,50
	de	20,80,10,20
	de	70,0,0,0
	de	0,0,0,0
	org	0x2100+120
	de	5,120,0,0	;1-2
	de	100,0,255,0	;ovverrides B2pi,drop,xsig,tstl
	de	100,90,20,0	;drop(2),sw
	de	0,0,0,0
	de	0,0,0,0
	de	0,0,0,0
	org	0x2100+144
	de	6,101,0,0	;PE-Night
	de	150,15,50,0
	de	73,0,5,50
	de	20,100,13,20
	de	70,200,8,100
	de	20,0,0,0
	endm
#endif

mdata	macro	ofs
	kset8	ofs
	call	tbl@read
	endm

Aset macro
	local	@10,@20,@21,@22,@100
	kstore8	kreg+8	;mode
	kset8	48
@10
	kstore8	madr
	ke2seta
	ke2read
	kcmpU8	kreg+8
	kjmpc	@20
	kset8	24
	kadd8	madr
	goto	@10
@20
	ksub8	kreg+8
	ke2adda
	ke2inca
	ke2read
	kstore8	mscr
@21
	mdata	4
	kstore8	B2pi
	kjmpns	mscr,0,@22
	bsf	oflag,1		;stoll control on
	mdata	5
	tstf	kreg
	kjmpz	@22
	kstore8	tstoll
@22
	kset8	6
	call	tbl@seta
	kset8	15
	ke2readn hscr
	goto	@100
tbl@seta
	kadd8	madr
	ke2seta
	return
tbl@read
	call	tbl@seta
	ke2read
	return
bf@get
	kload8	bfp
	kcvsU8
	ksub8	bfn
	return
k8@reduce
	kstore16 kreg+10
	kload16	kreg+8
	knegS16
	kstore16 kreg+8
	kload16	kreg+10
	kintegU16 kreg+8
	return
scu@get
	kload8	mscr
w10k@get
	kmulU8xU8 awsc
	kshlU16 3
	return
@100
	endm
Aaccel macro
	kload8	hscr
	kminU8	we
	kstore8 awsc
	kload8	lscr
	kadd8	awsc
	kstore8 awsc
	clrf	awsc+1
	call	scu@get
	kstore8	ausc
	kstore8	brkrate
	endm
Atop	macro
	kload8	pscr
	kexlU8
	kstore16 ahsc
	kmov8	hscr,vscr
	clrf	atsc
	clrf	ausg
	kclr16	aslu
	endm
Ascan	macro
	local	exit
	kload8	ausc
	kmaxU8	ausg
	kmaxU8	aslu
	kstore8	brkrate
	tstf	wscr
	kjmpz	exit
	decf	wscr,F
	kjmpnz	exit
	kmov8	vscr,xscr
exit
	endm
Adecel0 macro
	call	bf@get
	kstore8	abfo
	endm
Adecel1 macro	;get brake
	local	@110,@111,@112,@113,@120,@130,@131,@132,@133,@140
	kmov8	tpwm,tstoll	;sound off
	kjmps	mscr,0,@110
	clrf	tpwm		;sound on
@110;brake profile
	tstf	pscr
	kjmpnz	@111
	call	scu@get
	kstore8	ausc
	goto	@120
@111
	kload8	ahsc
	kmulU8xU8 rscr
	kcmpS8	abfo
	kjmpnc	@112
	kset8	0
	goto	@113
@112
	kload8	abfo
	knorm8	ahsc
	kmulU8xU8 hscr	;vscr
@113
	kstore8	vscr
	incf	atsc,F
	kload8	atsc
	kcmpU8	xscr
	kjmpnz	@120
	call	scu@get
	kstore8	ausc
	clrf	atsc
@120
	tstf	psig
	kjmpz	@130
	clrf	ausg
	kload8	psig
	knegU8
	kcmpS8	abfo
	kjmpnc	@130
	kload8	qsig
	knegU8
	kstore8	kreg+8
	kload8	ausc
	call	k8@reduce
	kstore8	ausg
@130;stoll check
	kjmpns	mscr,0,@140
	kload8	tmsec
	kcmpU8	dstl
	kjmpnc	@140
	kmulU8xU8 hstl
	kshlU16	2
	kstore8	aslh
;	kcmpS8	abfo
;	kjmpc	@132
	kload8	abfo
	ksub8	aslh
	kstore8	kreg+8
	kload8	vstl
	kjmpns	kreg+8,7,@131
	kload8	ustl
@131
	kmulU8xS8 kreg+8
	kstore16 kreg+8
	goto	@133
;@132;may touch the gnd
	kload8	ustl
	kexlU8
	knegU16
	kstore16 kreg+8
@133
	kload16 aslu
	call	k8@reduce
	kstore16 aslu
@140
	endm
Adecel2 macro
	local	@210,@220
	tstf	vscr
	kjmpz	@210
	kload8	ausc
	kmulU8xU8 vscr
	kmulU8xU8 awsc
	kshrU16 2
	kstore16 kreg+8
	kload8	sscr
	kexlU8
	kshrU16 5
	kmaxU16 kreg+8
	kstore16 kreg+8
	kload16 awsc
	call	k8@reduce
	kstore16 awsc
@210
	kload8	ahsc
	kmulU8xU8 qscr
	kstore16 kreg+8
	kload16 ahsc
	call	k8@reduce
	kstore16 ahsc
@220
	endm
Adecel3 macro
	endm
Awat	macro
	endm
