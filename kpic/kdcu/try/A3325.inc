	cblock
	madr,mscl
	zovr,hscl
;vars
	ausl:2
	arsg,ahsg
	atbl,atim,aflag
	endc

#ifdef M443	;D34-W24-H3(BassOne pla-spool)
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,245,200,170
	de	64,64,64,64
	de	120,200,30,0
	de	0,0,0,0
	de	255,210,120,70	;+16:brake profile
	de	50,40,30,0	;
	de	200,200,200,200	;+24:brake cut ratio
	de	200,200,200,0	;
	de	50,20,10,5	;+32:tension threshold
	de	5,5,5,0		;
	de	0,10,30,40	;+40:low pass
	de	50,50,50,0	;
	endm
#endif
#ifdef M414	;D34-W21-H4(Exsence pla-spool)
Adata macro
	org	0x2100+48
	de	6,240,220,200
	de	180,160,140,120
	de	140,30,0,0	;zero,thres
	de	255,100,20,20	;+12:score mapping
	de	25,255,240,210	;+16:brake profile
	de	150,90,60,40	;
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
#ifdef	OMEGASOUND
	kset8	3
	ke2adda
	ke2read
	kstore8	atim	;use as sound threshold
#endif
	kset8	8
	call	tbl@seta
	kset8	4
	ke2readn zovr
	kmov8	B2pi,zovr
	mdata	16
	kstore8	atbl
	kset8	6
	kmulU8xU8 atbl
	ktrmU16
	kstore8	atbl
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
	clrf	arsg
	kclr16	ausl
	kjmps	oflag,1,exit	;stoll control on
#ifdef	OMEGASOUND
	kload8	we
	kcmpU8	atim
	kjmpnc	exit
#endif
	clrf	tpwm	;sound on
exit
	clrf	zovr	;use as timer
	endm
Ascan	macro
	local	@10,@20,@30
	kload8	we
	kmulU8xU8 hscl
	kshlU16	2
	kstore8	ahsg
	kload16	be
	kshlS16 3
	kcmpS8	ahsg
	kjmpnc	@10
	incf	arsg,W
	btfss	STATUS,Z
	movwf	arsg
@10
	endm
Adecel0 macro
	kload8	zovr
	knorm8	atbl
	kstore8	atim
	endm
Adecel1 macro
	kset8	17
	call	tbl@seta
	kload8	atim
	kl1load	7
	kstore8	kreg+8
	kset8	12
	call	tbl@seta
	kload8	arsg
	kl1load	4
	kmulU8xU8 kreg+8
	kmulU8xU8 mscl
	kstore8	brkrate
	endm
Adecel2 macro
	endm
Adecel3 macro
	incf	zovr,W
	btfss	STATUS,Z
	movwf	zovr
	endm
Awat	macro
	local	odd,exit
	kjmps	kreg,0,odd
	kload8	arsg
	goto	exit
odd
	kload8	ahsg
exit
	endm
