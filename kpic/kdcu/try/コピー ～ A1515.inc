	cblock
	madr,mscl
	zovr,dscl,cscl,radp
	nadp,hadp
;vars
	ausl:2,awsc:2
	ausg,alsg,acsg
	atim,arev,aflag
	absum:2,abtrc
	endc

#ifdef M443	;D34-W24-H3(BassOne pla-spool)
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,200,180,160
	de	0,0,0,0
	de	100,10,80,70
	de	50,50,0,0
	de	255,210,100,20	;+16:brake upper
	de	30,10,0,0	;+20:cutoff tension
	endm
#endif
#ifdef M414	;D34-W21-H4(Exsence pla-spool)
Adata macro
	org	0x2100+48
	de	6,200,190,180
	de	170,160,150,140
	de	120,10,80,70
	de	50,50,0,0
	de	255,200,120,50	;+16:brake upper
	de	30,10,0,0	;+20:cutoff tension
	endm
#endif
#ifdef M4571	;D34-W25-H7(Scorpion) Type-I
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	4,200,200,200
	de	32,64,96,255
	de	140,10,80,70
	de	50,50,0,0
	de	255,210,100,20	;+16:brake upper
	de	30,10,0,0	;+20:cutoff tension
	endm
#endif
#ifdef M4572	;D34-W25-H7(Scorpion) Type-II
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	4,250,200,150
	de	0,0,0,0
	de	140,10,80,70
	de	50,50,0,0
	de	255,210,100,20	;+16:brake upper
	de	30,10,0,0	;+20:cutoff tension
	endm
#endif
#ifdef M022	;D30-W22-H2
Adata macro
	org	0x2100+48
	de	6,130,130,130
	de	130,130,130,130
	de	240,20,80,70
	de	50,50,0,0
	de	255,200,120,50	;+16:brake upper
	de	30,10,0,0	;+20:cutoff tension
	endm
#endif

mdata	macro	ofs
	kset8	ofs
	call	tbl@read
	endm
tdata	macro	ofs,var
	kset8	ofs
	call	tbl@seta
	kload8	atim
	kl1load	4
	kstore8	var
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
	kset8	48
	kadd8	madr
	goto	@10
@20
	ksub8	kreg+8
	ke2adda
	ke2inca
	ke2read
	kstore8	mscl
;	kstore8	B2pi	;test
#ifdef	OMEGASOUND
	kset8	3
	ke2adda
	ke2read
	kstore8	atim	;use as sound threshold
#endif
	kset8	8
	call	tbl@seta
	kset8	8
	ke2readn zovr
	kmov8	B2pi,zovr
;	kmov8	mscl,zovr ;test
	kjmpns	mscl,0,@22
	bsf	oflag,1	;stoll control on
@22
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
@100
	endm
Aaccel macro
	kmov8	brkrate,mscl
	endm
Atop	macro
	local	exit
	kclr16	ausl
	clrf	arev
	clrf	aflag
	clrf	abtrc
	kclr16	absum
	kmov16	awsc,we
	kjmps	oflag,1,exit	;stoll control on
#ifdef	OMEGASOUND
	kload8	we
	kcmpU8	atim
	kjmpnc	exit
#endif
	clrf	tpwm	;sound on
exit
	clrf	zovr	;use as time count
	endm
Ascan	macro
	local	@10,@20,@30,@40,@99
	bcf	aflag,0
	kload16	be
	kshlS16 3
	kcmpS8	acsg
	kjmpc	@10
	kload8	ausg
	goto	@20
@10
	kload8	alsg
	bsf	aflag,0
@20
	kmulU8xU8 mscl
	kstore8	brkrate

	kjmps	aflag,2,@30
	incf	arev,F
	kload8	arev
	kcmpU8	radp
	kjmpnc	@99
	bsf	aflag,1
	kjmps	aflag,0,@99
	bsf	aflag,2
	clrf	arev
	goto	@99
@30
	incf	arev,W
	btfss	STATUS,Z
	movwf	arev
	kload16	be
	kshrS16 5
	kcvsS16
	kstore16 kreg+8
	kload16	absum
	kintegU16 kreg+8
	kstore16 absum
@99
	endm
Adecel0 macro
	local	@010
	kload8	ausg
	kmulU8xU8 dscl
	kstore16 kreg+8
	kload8	awsc
	kmulU8xU16 kreg+8
	kstore16 kreg+8	;max w drop
	kload16	awsc
	kcmpU16	we
	kjmpnc	@010
	ksub16	we
	kminU16	kreg+8
	kstore16 kreg+8	;awsc drop
	kload16	awsc
	ksub16	kreg+8
	kstore16 awsc
@010
	kload8	awsc
	knorm8	wtop
	comf	kreg,F
	kstore8	atim
	endm
Adecel1 macro
	kload8	arev
	kmaxU8	nadp
	kexlU8
	kstore16 kreg+8
	kload16	absum
	knorm16	kreg+8
	kstore8	abtrc
	endm
Adecel2 macro	;get brake
	local	@299
	tdata	16,ausg
	tdata	20,acsg
	kload8	ausg
	kmulU8xU8 cscl
	kstore8	alsg
	kjmpns	aflag,2,@299
	kload8	abtrc
	knorm8	hadp
	kstore8	kreg+8
	kload8	ausg
	ksub8	alsg
	kmulU8xU8 kreg+8
	kstore8	kreg+8
	kload8	ausg
	ksub8	kreg+8
	kstore8	ausg
@299
	endm
Adecel3 macro
	incf	zovr,W
	btfss	STATUS,Z
	movwf	zovr
	endm
Awat	macro
	local	odd,exit
	kjmps	kreg,0,odd
	kload8	abtrc
	goto	exit
odd
exit
	endm
