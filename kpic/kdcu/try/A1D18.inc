	cblock
	madr,mscl
	zovr,tscl
;vars
	ausl:2
	ausg,ahsg,ansg
	abfo,abfa,aprb,atim
	endc

#ifdef M443	;D34-W24-H3 bassOne
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,200,160,120
	de	64,64,135,75
	de	255,150,130,40	;brake profile
	de	150,200,200,200	;brake cut ratio
	de	200,30,20,5	;tension threshold
	de	1,15,25,30	;evalu width
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
	kstore8	abfo	;use as sound threshold
#endif
	kset8	6
	call	tbl@seta
	kset8	2
	ke2readn zovr
	kmov8	B2pi,zovr
	kjmpns	mscl,0,@22
	bsf	oflag,1	;stoll control on
@22
;init ring buffer
	call	rb@init
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
rb@init
	clrf	aprb
	clrf	kreg
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
	kclr16	ausl
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
	kload8	bfp
	kcvsU8
	ksub8	bfn
	kstore8	abfo
	kload8	zovr
	knorm8	tscl
	kstore8	atim
	endm
Adecel1 macro	;get brake
	local	@102,@103,@110,@120
	kset8	16
	call	tbl@seta
	kload8	atim
	kl1load	4
	kstore8	ahsg	;tension ref
	kminU8	bfp
	call	rb@push
	kset8	20
	call	tbl@seta
	kload8	atim
	kl1load	4
	kstore8	ansg	;eval time
	clrf	kreg+8	;eval time count
	clrf	kreg
	call	rb@pop
	kexmU8
	kstore16 kreg+10
@103
	kload8	kreg+8
	kcmpU8	ansg
	kjmpc	@110
	incf	kreg,F
	call	rb@pop
	kexmU8
	kadd16	kreg+10
	kstore16 kreg+10
	incf	kreg+8,F
	goto	@103
@110
	incf	kreg,F
	kexlU8
	kstore16 kreg+8
	kload16 kreg+10
	knorm16 kreg+8
	kstore8	abfa
	endm
Adecel2 macro
	kset8	8
	call	tbl@seta
	kload8	atim
	kl1load	4
	kstore8	ausg	;base brake profile
	kset8	12
	call	tbl@seta
	kload8	atim
	kl1load	4
	kstore8	kreg+8	;brake cut
	kload8	abfa
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
	kload8	abfa
	goto	exit
odd
	kload8	ahsg
exit
	endm
