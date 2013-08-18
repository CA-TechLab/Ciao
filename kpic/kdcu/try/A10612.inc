	cblock
	madr,mscl
	zovr,bpww,bpta,bpim
;vars
	ausg,abpy:2,abpi:2
	atim:2
	endc

#ifdef M443	;D34-W24-H3(BassOne pla-spool)
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,200,180,160
	de	0,0,0,0
	de	100,60,20,100
	de	0,0,0,0
	de	255,240,170,150	;+16:brake upper
	de	30,10,10,0	;+20:cutoff tension
	endm
#endif
#ifdef M414	;D34-W21-H4(Exsence pla-spool)
Adata macro
	org	0x2100+48
	de	6,200,190,180
	de	170,160,150,140
	de	120,100,30,100
	de	0,0,0,0
	de	255,230,180,150	;+16:brake table
	de	120,90,60,40
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
tdata	macro	ofs
	kset8	ofs
	call	tbl@seta
	kload8	zovr
	kl1load	11
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
	kclr16	abpy
	kclr16	abpi
	kjmps	oflag,1,exit	;stoll control on
#ifdef	OMEGASOUND
	kload8	we
	kcmpU8	atim
	kjmpnc	exit
#endif
	clrf	tpwm	;sound on
exit
	clrf	zovr	;use as time count
	kclr16	atim
	kset8	255
	kstore8	ausg
	endm
Ascan	macro
	local	exit
	kload16	tcount
	kshlU16	3
	kstore8	kreg+11
	kload16 abpy
	ksub16	be
	kstore16 kreg+8
	kload8	bpta
	kmulU8xS16 kreg+8
	kstore24 kreg+8
	kload8	bpww
	kmulU8xS16 abpi
	kshlS24	2
	kadd24 kreg+8
	kshrS24	3
	ktrmS24
	kstore16 kreg+8
	kload8	kreg+11
	kmulU8xS16 kreg+8
	kshrS16	2
	knegS16
	kadd16	abpy
	kstore16 abpy

	kload8	kreg+11
	kmulU8xS16 abpy
	kstore16 kreg+8
	kshrS16	2
	kadd16 abpi
	kstore16 abpi

	kjmps	kreg+8,7,exit
	kadd16	atim
	kstore16 atim
exit
	endm
Adecel0 macro
	nop
	endm
Adecel1 macro
	nop
	endm
Adecel2 macro
	tdata	16
	kmulU8xU8 mscl
	kstore8	brkrate
	endm
Adecel3 macro
	incf	zovr,W
	btfss	STATUS,Z
	movwf	zovr
	endm
Awat	macro
	local	odd,exit
	kjmps	kreg,0,odd
	kload8	atim
	goto	exit
odd
	kload16	abpy
	kshlS16	3
exit
	endm
