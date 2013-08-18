	cblock
	madr,mscr
	hscr,lscr
	vscr,wscr,xscr,pscr
	ksig,hsig,vsig,ysig
	hstl,dstl,ustl,vstl
;vars
	abfo
	ausl:2,ahsl
	awsc:2,ausc,atsc
	ausg,atsg,axsg,aysg,ahsg
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
	de	150,15,50,0
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
	de	150,15,50,0
	de	80,0,0,5
	de	80,40,10,10
	de	200,8,100,20
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
	goto	@100
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
	local	exit
	clrf	atsc
	clrf	axsg
	incf	axsg,F	;axsg=1
	clrf	atsg
	clrf	aysg
	kclr16	ausl
	clrf	ahsl
	kjmps	mscr,0,exit
	clrf	tpwm		;sound on
exit
	endm
Ascan	macro
	local	exit
	tstf	wscr
	kjmpz	exit
	decf	wscr,F
	kjmpnz	exit
	kmov8	vscr,xscr
exit
	endm
Adecel0 macro
	kload8	bfp
	kcvsU8
	ksub8	bfn
	kstore8	abfo
	endm
Adecel1 macro	;get brake
	local	@110
	kload8	ausc
	kmulU8xU8 vscr
	kmulU8xU8 awsc
	kshrU16 2
	kstore16 kreg+8
	kload16 awsc
	call	k8@reduce
	kstore16 awsc
	incf	atsc,F
	kload8	atsc
	kcmpU8	pscr	;hold time
	kjmpnc	@110
	call	scu@get
	kstore8	ausc
	clrf	atsc
@110
	endm
Adecel2 macro
	local	@203,@204,@210,@220
	clrf	ausg
	kload8	awsc
;	kminU8	we
	kmulU8xU8 hsig
	kshlU16	3
	kstore8	ahsg
	kcmpU8	bfp
	kjmpc	@203
	clrf	atsg
	goto	@210
@203
	kload8	atsg
	kcmpU8	axsg
	kjmpc	@204
	incf	atsg,F
	goto	@210
@204
	kload8	ahsg
	kmov8	kreg+8,vsig
	call	k8@reduce
	kcmpU8	bfp
	kjmpnc	@210
	kload8	ahsg
	ksub8	bfp
	kmulU8xU8 ksig
	kshlU16 5
	kstore8	ausg
	kjmps	abfo,7,@210
	kmov8	kreg+8,vsig
	call	k8@reduce
	kstore8	ausg
@210
	incf	aysg,F	;extend feedback delay
	kload8	aysg
	kcmpU8	ysig
	kjmpnc	@220
	incf	axsg,F
	clrf	aysg
@220
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
	kload8	vstl
	kjmpns	kreg+8,7,@302
	kload8	ustl
@302
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
	endm
