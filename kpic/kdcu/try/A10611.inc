	cblock
	madr,mscl
	zovr,cswt,rswt,wadp
	nadp
;vars
	ausg,alsg,acsg,aasg:2,ansg
	atim,aflag
	abmax,abwid:2
	endc

#ifdef M443	;D34-W24-H3(BassOne pla-spool)
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,200,180,160
	de	0,0,0,0
	de	100,170,5,100
	de	150,0,0,0
	de	255,240,220,200	;+16:brake upper
	de	30,10,10,0	;+20:cutoff tension
	endm
#endif
#ifdef M414	;D34-W21-H4(Exsence pla-spool)
Adata macro
	org	0x2100+48
	de	6,200,190,180
	de	170,160,150,140
	de	120,170,5,100
	de	150,0,0,0
	de	255,240,220,200	;+16:brake upper
	de	30,10,10,0	;+20:cutoff tension
	endm
#endif
#ifdef M4571	;D34-W25-H7(Scorpion) Type-I
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	4,200,200,200
	de	32,64,96,255
	de	140,170,100,150
	de	0,0,0,0
	de	255,230,210,200	;+16:brake upper
	de	30,10,10,0	;+20:cutoff tension
	endm
#endif
#ifdef M022	;D30-W22-H2
Adata macro
	org	0x2100+48
	de	6,130,130,130
	de	130,130,130,130
	de	240,170,100,150
	de	0,0,0,0
	de	255,230,210,200	;+16:brake upper
	de	30,10,10,0	;+20:cutoff tension
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
ausg@cut
	kmulU8xU8 ausg
	kstore8	kreg+8
	kload8	ausg
	ksub8	kreg+8
	return
@100
	endm
Aaccel macro
	kmov8	brkrate,mscl
	endm
Atop	macro
	local	exit
	clrf	aflag
	clrf	abmax
	clrf	ansg
	kclr16	abwid
	kclr16	aasg
	kjmps	oflag,1,exit	;stoll control on
#ifdef	OMEGASOUND
	kload8	we
	kcmpU8	atim
	kjmpnc	exit
#endif
	clrf	tpwm	;sound on
exit
	clrf	zovr	;use as time count
	clrf	atim
	tdata	16,ausg
	tdata	20,acsg
	endm
Ascan	macro
	local	@10,@20,@30,@40,@99
	bcf	aflag,0
	kload8	tcount+1
	swapf	kreg,F ;random number
	kmulU8xU8 rswt
	kadd8	acsg
	kstore8	ansg	;threshold
	kload16	be
	kshlS16 3
	kcmpS8	ansg
	kjmpnc	@10
	kload16	be
	kcvsS16
	kshlU16 3
	kmaxU8	abmax
	kstore8	abmax
	kload16	abwid
	kintegU16 tcount
	kstore16 abwid
	bsf	aflag,0
	kload8	alsg
	goto	@20
@10
	kload8	ausg
@20
	kmulU8xU8 mscl
	kstore8	brkrate
@99
	endm
Adecel0 macro
	kset8	75	;1.5sec
	kstore8	kreg+8
	kload8	zovr
	knorm8	kreg+8
	kstore8	atim
	endm
Adecel1 macro
	local	@100,@199
	kjmps	aflag,0,@199
	kload8	abmax
	kcmpU8	acsg
	kjmpnc	@100
	ksub8	acsg
	kstore8	kreg+10
	kload8	abwid
	knorm8	wadp
	kstore8	kreg+8
	kmulU8xU8 kreg+8
	kstore8 kreg+8
	kset8	255
	ksub8	kreg+8
	kmulU8xU8 kreg+10
	kstore16 kreg+8
	kload16	aasg
	kintegU16 kreg+8
	kstore16 aasg
@100
	clrf	abmax
	kclr16	abwid
@199
	endm
Adecel2 macro	;get brake
	tdata	16,ausg
	tdata	20,acsg
	kload8	aasg
	knorm8	nadp
	call	ausg@cut
	kstore8	ausg

	kload8	cswt
	call	ausg@cut
	kstore8	alsg
	endm
Adecel3 macro
	incf	zovr,W
	btfss	STATUS,Z
	movwf	zovr
	endm
Awat	macro
	local	odd,exit
	kjmps	kreg,0,odd
	kload8	ansg
	goto	exit
odd
	kload8	aasg
exit
	endm
