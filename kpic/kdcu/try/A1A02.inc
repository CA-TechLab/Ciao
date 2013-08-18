	cblock
	madr,mscl
	wsig,tsig,hsig,nsig
	wpse,spse,dpse:2
	hstl,dstl,ustl
;vars
	atim,abfo
	ausl:2,ahsl
	awps:2,aups
	atsg,ansg,ausg
	aprb
	endc

#ifdef M414	;D34-W21-H4
Adata macro
	org	0x2100+48
	de	6,201,163,125
	de	200,160,120,100
	de	150,15,0,0
	de	50,100,70,30
	de	50,130,80,0
	de	200,8,100
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
	kstore8	mscl
@21
	mdata	8
	kstore8	B2pi
	kjmpns	mscl,0,@22
	bsf	oflag,1		;stoll control on
	mdata	9
	tstf	kreg
	kjmpz	@22
	kstore8	tstoll
@22
	kset8	12
	call	tbl@seta
	kset8	12
	ke2readn wsig
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
	kmov8	brkrate,mscl
	endm
Atop	macro
	local	exit
	kmov8	awps,wpse
	clrf	awps+1
	clrf	atim
	kclr16	ausl
	clrf	ahsl
	clrf	aups
	kjmps	mscl,0,exit
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
	kload8	atim
	knorm8	tsig
	kstore8	atsg
	incf	atim,W
	btfss	STATUS,Z
	movwf	atim
	endm
Adecel1 macro	;get brake
	kload8	aups
	kmulU8xU8 dpse
	kmulU8xU8 awps
	kshrU16 2
	kstore16 kreg+8
	kload16 awps
	call	k8@reduce
	kstore16 awps
	knorm8	wpse
	kmulU8xU8 mscl
	kmulU8xU8 spse
	kstore8	aups
	endm
Adecel2 macro
	local	@202,@203,@210,@220
	kload8	atsg
	kmulU8xU8 atsg
	kstore8	kreg+8
	kmulU8xU8 nsig
	kstore8	ansg
	kset8	255
	ksub8	kreg+8
	kmulU8xU8 hsig
	ksub8	bfp
	kjmpns	kreg,7,@202
	clrf	kreg
@202
	knorm8	hsig
	kmulU8xU8 mscl
	kstore8	ausg
	call	rb@push
	clrf	kreg+8
@203
	kload8	kreg+8
	kcmpU8	ansg
	kjmpc	@210
	call	rb@pop
	kminU8	ausg
	kstore8	ausg
	incf	kreg+8,F
	goto	@203
@210
	endm
Adecel3 macro
	local	@301,@302,@310
	kjmpns	mscl,0,@310
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
	kload8	aups
	kmaxU8	ausg
	kmaxU8	ausl
	kstore8	brkrate
	endm
Awat	macro
	kload8	ansg
	endm
