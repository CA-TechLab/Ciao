	cblock
	madr,mscl
	zovr,dscl,cscl
;vars
	ausl:2,awsc:2
	ausg,alsg,acsg,axsg,ansg
	abff:2
	atim
	endc

#ifdef M443	;D34-W24-H3(BassOne pla-spool)
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,240,200,170
	de	64,64,64,64
	de	120,25,85,100
	de	0,0,0,0
	de	255,210,130,80	;+16:brake upper
	de	0,20,35,50	;+20:tension low pass
	de	30,10,0,0	;+24:cutoff tension
	de	50,20,10,10	;+28:catenary tension
	endm
#endif
#ifdef M414	;D34-W21-H4(Exsence pla-spool)
Adata macro
	org	0x2100+48
	de	6,240,220,200
	de	180,160,140,120
	de	150,25,85,100
	de	0,0,0,0
	de	255,210,130,80	;+16:brake upper
	de	0,20,35,50	;+20:tension low pass
	de	30,10,0,0	;+24:cutoff tension
	de	50,20,10,10	;+28:catenary tension
	endm
#endif
#ifdef M4571	;D34-W25-H7(Scorpion) Type-I
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	4,200,200,200
	de	32,64,96,255
	de	140,25,100,0
	de	0,0,0,0
	de	255,210,100,70	;+16:brake profile
	de	40,30,25,0	;
	de	200,200,200,200	;+24:brake cut ratio
	de	200,200,200,0	;
	de	50,20,10,5	;+32:tension threshold
	de	5,5,5,0		;
	de	0,10,30,40	;+40:low pass
	de	50,50,50,0	;
	endm
#endif
#ifdef M4572	;D34-W25-H7(Scorpion) Type-II
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	4,250,200,150
	de	0,0,0,0
	de	140,25,100,0
	de	0,0,0,0
	de	255,210,100,70	;+16:brake profile
	de	40,30,25,0	;
	de	200,200,200,200	;+24:brake cut ratio
	de	200,200,200,0	;
	de	50,20,10,5	;+32:tension threshold
	de	5,5,5,0		;
	de	0,10,30,40	;+40:low pass
	de	50,50,50,0	;
	endm
#endif
#ifdef M022	;D30-W22-H2
Adata macro
	org	0x2100+48
	de	6,130,130,130
	de	130,130,130,130
	de	220,25,100,0
	de	0,0,0,0
	de	255,210,100,70	;+16:brake profile
	de	40,30,25,0	;
	de	200,200,200,200	;+24:brake cut ratio
	de	200,200,200,0	;
	de	50,20,10,5	;+32:tension threshold
	de	5,5,5,0		;
	de	0,10,30,40	;+40:low pass
	de	50,50,50,0	;
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
	kmov8	apfm,zovr+3
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
	kclr16	abff
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
	local	@10,@20
	kload16	be
	kshlS16 3
	kcmpS8	acsg
	kjmpc	@10
	kload8	ausg
	goto	@20
@10
	kload8	alsg
@20
	kmulU8xU8 mscl
	kstore8	brkrate
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
Adecel1 macro	;get brake
	tdata	20,ansg	;low pass filter
	tdata	24,acsg	;cutoff thres
	tdata	28,axsg	;catenary thres
	endm
Adecel2 macro
	tdata	16,ausg	;higher limit
	kload8	ausg
	kmulU8xU8 cscl
	kstore8	alsg
	kload8	ausg
	ksub8	alsg
	kstore8	kreg+8
	kload8	abff
	kcvsS8
	knorm8	axsg
	kmulU8xU8 kreg+8
	kstore8	kreg+8
	kload8	ausg
	ksub8	kreg+8
	kstore8	ausg
	endm
Adecel3 macro
	kload8	bfp
	kcvsU8
	ksub8	bfn
	ksub8	abff
	kstore8	kreg+8
	kload8	ansg
	kmulU8xS8 kreg+8
	kstore16 kreg+8
	kload16	abff
	kintegS16 kreg+8
	kstore16 abff

	incf	zovr,W
	btfss	STATUS,Z
	movwf	zovr
	endm
Awat	macro
	local	odd,exit
	kjmps	kreg,0,odd
	kload8	axsg
	goto	exit
odd
	kload8	abff
exit
	endm
