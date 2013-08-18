	cblock
	madr,mscl
	sghi,sghd,sglo,sgld
	sgth,sgtd,bpwq,bpta
	bpks
;vars
	abpy:2,abpi:2
	ashd,asld,astd
	asgy,asgk
	aflg
	endc

#ifdef M443	;D34-W24-H3(BassOne pla-spool)
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,200,180,160
	de	0,0,0,0
	de	100,110,80,20
	de	30,50,78,5
	de	30,0,0,0
	endm
#endif
#ifdef M414	;D34-W21-H4(Exsence pla-spool)
Adata macro
	org	0x2100+48
	de	6,200,190,180
	de	170,160,150,140
	de	120,110,80,20
	de	30,50,60,15
	de	40,0,0,0
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
	kload8	arev
	kl1load	4
	endm

Aset macro
	local	@10,@20,@21,@22,@100
	kstor8	kreg+8	;mode
	kset8	48
@10
	kstor8	madr
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
	kstor8	mscl
;	kstor8	B2pi	;test
#ifdef	OMEGASOUND
	kset8	3
	ke2adda
	ke2read
	kstor8	aflg	;use as sound threshold
#endif
	kset8	8
	call	tbl@seta
	kset8	12
	ke2readn sghi
	kmov8	B2pi,sghi
	kset8	255
	kstor8	sghi
	kmov8	asld,sgld
	kmov8	ashd,sghd
	kmov8	astd,sgtd
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
	kcmpU8	aflg
	kjmpnc	exit
#endif
	clrf	tpwm	;sound on
exit
	clrf	aflg
	clrf	asgy
	kset8	255
	kstor8	asgk
	endm
Ascan	macro
	local	bpf,nelo,nehi,brhi,brlo,brtr,exit
	kload16	tcount
	kshlU16	3
	kstor8	kreg+11
bpf
	kload16 be
	kshlU16 2
	kstore8	kreg+8
	kload16 abpy
	ksub16	kreg+8
	kstore16 kreg+8
	kload8	bpta
	kmulU8xS16 kreg+8
	kstore24 kreg+8
	kload8	bpwq
	kmulU8xS16 abpi
	kshlS24	3
	kadd24 kreg+8
	kstore16 kreg+8
	kload8	kreg+11
	kmulU8xS16 kreg+8
	knegS16
	kadd16	abpy
	kstore16 abpy

	kload8	kreg+11
	kmulU8xS16 abpy
	kadd16 abpi
	kstore16 abpi

	kload16	abpy
	kcvsS16
	kshlU16 3
	kmaxU8 	asgy
	kstor8	asgy	;trace max

	kload16	be
	kshlS16	3
	kcmpS16	sgth
	kjmpnc	brhi
	bcf	aflg,0
	kload8	sglo
	goto	exit
brhi
	kload8	sghi
	kmaxU8	sglo
	kminU8	asgk
	kjmps	aflg,0,exit
	kload8	asgy
	kmulU8xU8 bpks
	kshlU16	3
	kminU8	sghi
	kstor8	kreg+8
	kload8	sghi
	ksub8	kreg+8
	kmaxU8	sglo
	kminU8	asgk
	kstor8	asgk
	clrf	asgy
	bsf	aflg,0
	goto	brhi
exit
	kmulU8xU8 mscl
	kstor8	brkrate

	kload8	ashd
	kexmU8
	kminU16	sghi
	kstor16	kreg+8
	kload16	sghi
	ksub16 kreg+8
	kstor16 sghi

	kload8	asld
	kexmU8
	kminU16 sglo
	kstor16	kreg+8
	kload16	sglo
	ksub16 kreg+8
	kstor16 sglo

	kload8	astd
	kexmU8
	kminU16 sgth
	kstor16	kreg+8
	kload16	sgth
	ksub16 kreg+8
	kstor16 sgth
	endm
Adecel0 macro
	endm
Adecel1 macro
	endm
Adecel2 macro
	endm
Adecel3 macro
	endm
Awat	macro
	local	odd,exit
	kjmps	kreg,0,odd
	kload8	asgy
	goto	exit
odd
	kload16	abpy
	kshlS16	3
exit
	endm
