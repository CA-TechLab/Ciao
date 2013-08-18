	cblock
	madr,mscl
	zovr,tscl
;vars
	ausl:2
	ausg,ahsg,ansg
	abfo,abff:2
	atim
	endc

#ifdef M443	;D34-W24-H3 bassOne
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,250,200,170
	de	64,64,64,64
	de	100,100,0,0
	de	0,0,0,0
	de	255,170,130,80	;+16:brake profile
	de	40,0,0,0	;
	de	130,170,200,200	;+24:brake cut ratio
	de	200,0,0,0	;
	de	30,20,10,5	;+32:tension threshold
	de	5,0,0,0		;
	de	0,30,30,30	;+40:low pass
	de	30,0,0,0	;
	endm
#endif
#ifdef M414	;D34-W21-H4 exsence
Adata macro
	org	0x2100+48
	de	6,201,181,161
	de	200,180,160,140
	de	150,30,70,100
	de	85,0,20,100
	de	50,40,15,10	;tension threshold
	de	0,5,15,20	;feedback delay
	endm
#endif
#ifdef M4571	;D34-W25-H7-1 Scorpion type-I
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	4,200,200,200
	de	32,64,96,255
	de	140,30,70,100
	de	85,0,20,100
	de	50,40,15,10	;tension threshold
	de	0,5,15,20	;feedback delay
	endm
#endif
#ifdef M724	;D37-W22-H4
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,240,240,240
	de	64,64,64,64
	de	120,100,85,0
	de	75,20,100,0
	de	50,40,15,7	;tension threshold
	de	0,10,20,25	;feedback delay
	endm
#endif
#ifdef M022	;D30-W22-H2
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,110,110,110
	de	80,80,80,80
	de	240,100,85,0
	de	75,20,100,0
	de	50,40,15,7	;tension threshold
	de	0,10,20,25	;feedback delay
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
	kstore8	abfo	;use as sound threshold
#endif
	kset8	8
	call	tbl@seta
	kset8	2
	ke2readn zovr
	kmov8	B2pi,zovr
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
	kmov8	ausg,mscl
	kjmps	oflag,1,exit	;stoll control on
#ifdef	OMEGASOUND
	kload8	we
	kcmpU8	abfo
	kjmpnc	exit
#endif
	clrf	tpwm	;sound on
exit
	clrf	zovr	;use as timer
	endm
Ascan	macro
	endm
Adecel0 macro
	local	@010
	kload8	zovr
	knorm8	tscl
	kstore8	atim
	endm
Adecel1 macro	;get brake
	local	@102,@103,@110,@120
	kset8	32
	call	tbl@seta ;tension threshold
	kload8	atim
	kl1load	5
	kstore8	ahsg
	kload8	bfp
	kcvsU8
	ksub8	bfn
	kstore8	abfo
	ksub8	abff
	kstore8	kreg+8
	kset8	40
	call	tbl@seta ;low pass filter
	kload8	atim
	kl1load	5
	kmulU8xS8 kreg+8
	kstore16 kreg+8
	kload16	abff
	kintegS16 kreg+8
	kstore16 abff
	endm
Adecel2 macro
	kset8	16
	call	tbl@seta ;brake profile
	kload8	atim
	kl1load	5
	kstore8	ausg
	kset8	24
	call	tbl@seta ;brake cut
	kload8	atim
	kl1load	5
	kstore8	kreg+8
	kset8	0
	kmaxS8	abff
	kmaxU8	bfp
	knorm8	ahsg
	kmulU8xU8 kreg+8
	kmulU8xU8 ausg
	kstore8	kreg+8
	kload8	ausg
	ksub8	kreg+8
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
	kload8	ahsg
	goto	exit
odd
	kload8	abff
exit
	endm
