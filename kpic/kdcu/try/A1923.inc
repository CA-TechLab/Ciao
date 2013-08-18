	cblock
	madr,mscr
	hscr,dscr
	ksig,hsig,dsig,vsig
	hstl,dstl,ustl
;vars
	abfo
	ausl:2,ahsl
	awsc:2,ausc
	ahsg:2,ausg,avsg:2
	aprb
	endc

#ifdef M427	;D34-W22-H7
Adata macro
	org	0x2100+48
	de	1,80,40,0	;1-2
	de	0,0,0,0
	de	150,0,255,0	;ovverrides B2pi,drop,xsig,tstl
	de	100,130,20,0	;drop(2),sw
	de	0,0,0,0
	de	0,0,0,0
	de	0,0,0,0
	org	0x2100+72
	de	3,80,60,0	;3-4
	de	0,0,0,0
	de	150,0,255,13	;ovverrides B2pi,drop,xsig,tstl
	de	20,125,40,0	;drop(2),sw
	de	0,0,0,0
	de	0,0,0,0
	de	0,0,0,0
	org	0x2100+96
	de	4,100,100,0	;F-mode
	de	0,0,0,0
	de	100,0,255,0
	de	80,0,0,5
	de	80,40,10,15
	de	200,8,100,20
	org	0x2100+120
	de	5,120,0,0	;1-2
	de	0,0,0,0
	de	100,0,255,0	;ovverrides B2pi,drop,xsig,tstl
	de	100,90,20,0	;drop(2),sw
	de	0,0,0,0
	de	0,0,0,0
	de	0,0,0,0
	org	0x2100+144
	de	6,101,0,0	;PE-Night
	de	0,0,0,0
	de	150,15,50,80
	de	80,0,0,5
	de	80,40,10,15
	de	200,8,100,20
	endm
#endif
#ifdef M414	;D34-W21-H4
Adata macro
	org	0x2100+48
	de	6,101,81,61
	de	100,80,60,50
	de	150,15,50,80
	de	50,50,7,30
	de	200,8,100,0
	endm
#endif
#ifdef M724	;D37-W22-H4
Adata macro
	org	0x2100+48
	de	6,121,101,81
	de	120,100,80,60
	de	120,20,50,0
	de	80,0,0,5
	de	80,30,10,10
	de	200,8,100,20
	endm
#endif
#ifdef M022	;D30-W22-H2
Adata macro
	org	0x2100+48
	de	6,70,60,50
	de	40,30,20,10
	de	240,0,60,0
	de	100,0,0,5
	de	80,50,10,10
	de	200,8,100,20
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
	mdata	8
	kstore8	B2pi
	kjmpns	mscr,0,@22
	bsf	oflag,1		;stoll control on
	mdata	9
	tstf	kreg
	kjmpz	@22
	kstore8	tstoll
@22
	kset8	10
	call	tbl@seta
	kset8	14
	ke2readn hscr
;var arrange
	kload8	ksig
	kmulU8xU8 mscr
	kshrU16 2
	knorm8	hsig
	kstore8	ksig
;init ring buffer
	goto	@100
	call	rb@init
tbl@seta
	kadd8	madr
	ke2seta
	return
tbl@read
	call	tbl@seta
	ke2read
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
aw@norm
	kmulU8xU8 awsc
	kshlU16 3
	return
rb@init
	clrf	aprb
	clrf	kreg
	decf	kreg,F	;kreg=255
rb@ini@lp
	call	rb@push
	kjmpns	aprb,6,rb@ini@lp
	return
rb@push
	movfw	aprb
	andlw	0x1F
	addlw	0xA0
	movwf	FSR
	kmov8	INDF,kreg
	incf	aprb,F
	return
rb@pop
	comf	kreg,W
	addwf	aprb,W
	andlw	0x1F
	addlw	0xA0
	movwf	FSR
	kmov8	kreg,INDF
	return
@100
	endm
Aaccel macro
	kload8	hscr
	kminU8	we
	kstore8 awsc
	clrf	awsc+1
	call	scu@get
	kstore8	ausc
	kstore8	brkrate
	endm
Atop	macro
	local	exit
	kclr16	avsg
	kclr16	ausl
	clrf	ahsl
	kload8	hsig
	call	aw@norm
	kstore8 hsig
	kset8	255
	kstore8	ahsg
	kjmps	mscr,0,exit
	clrf	tpwm
exit
	endm
Ascan	macro
	endm
Adecel0 macro
	kload8	bfp
	kcvsU8
	ksub8	bfn
	kstore8	abfo
	endm
Adecel1 macro	;get brake
	kload8	ausc
	kmulU8xU8 dscr
	kmulU8xU8 awsc
	kshrU16 2
	kstore16 kreg+8
	kload16 awsc
	call	k8@reduce
	kstore16 awsc
	call	scu@get
	kstore8	ausc

	kload16	tmsec
	kshlU16	5
	kmulU8xU8 dsig
	kstore16 kreg+8
	kload16 ahsg
	call	k8@reduce
	kstore16 ahsg
	endm
Adecel2 macro
	local	@202,@203,@210,@220
	kload8	ahsg
	kmulU8xU8 hsig
	ksub8	bfp
	kjmpns	kreg,7,@202
	clrf	kreg
@202
	kmulU8xU8 ksig
	kshlU16 5
	kstore8	ausg
	call	rb@push
	clrf	kreg+8
@203
	kload8	kreg+8
	kcmpU8	avsg
	kjmpc	@210
	call	rb@pop
	kminU8	ausg
	kstore8	ausg
	incf	kreg+8,F
	goto	@203
@210
	kload16	tmsec
	kshlU16	5
	kmulU8xU8 bfp
	kstore16 kreg+8
	kload8	vsig
	kmulU8xU16 kreg+8
	kshrU16	2
	kstore16 kreg+8
	kload16 avsg
	kintegU16 kreg+8
	kstore16 avsg
	endm
Adecel3 macro
	local	@301,@302,@310
	kjmpns	mscr,0,@310
	kload8	tmsec
	kcmpU8	dstl
	kjmpnc	@301
	kmulU8xU8 hstl
	kshlU16	2
	kstore8	ahsl
@301
	kload8	abfo
	ksub8	ahsl
	kstore8	kreg+8
	kload8	ustl
	kmulU8xS8 kreg+8
	kstore16 kreg+8
	kload16 ausl
	call	k8@reduce
	kstore16 ausl
	kcmpL8	250
	kjmpnc	@310
	clrf	tpwm
@310
	kload8	ausc
	kmaxU8	ausg
	kmaxU8	ausl
	kstore8	brkrate
	endm
Awat	macro
	kload8	avsg
	endm
