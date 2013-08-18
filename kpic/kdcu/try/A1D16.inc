	cblock
	madr,mscl
	zovr,csig,ksig,tstl
	hstl,ustl
;vars
	ausl:2
	ausg,aksg,ahsg,ansg
	abfo,aprb
	endc

#ifdef M443	;D34-W24-H3 bassOne
#define	OMEGASOUND
Adata macro
	org	0x2100+48
	de	6,200,160,120
	de	64,64,64,64
	de	135,85,0,75
	de	20,100,30,100
	de	50,50,10,15	;tension threshold
	de	75,0,0,15	;feedback delay
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
	kset8	8
	call	tbl@seta
	kset8	6
	ke2readn zovr
	kmov8	B2pi,zovr
	kjmpns	mscl,0,@22
	bsf	oflag,1	;stoll control on
@22
;adapt feedback gain
	mdata	16
	kexmU8
	kshlU16	3
	kstore16 kreg+8
	kload8	mscl
	kexmU8
	knorm16	kreg+8
	kstore8	aksg	;feedback
;adapt ratio
	kset8	255
	ksub8	csig
	kstore8	csig
	kload8	ksig
	kmulU8xU8 mscl
	kstore8	ksig
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
ttf@get
	kstore8	kreg+8	;time elapsed
	ke2read
	kstore8	kreg+9	;time clip
	ke2inca
	ke2read
	kstore8	kreg+10	;time span
	ke2inca
	kload8	kreg+8
	kcmpU8	kreg+9
	kjmpc	ttf@get@10
	knorm8	kreg+9
	kl1load	2
	return
ttf@get@10
	ke2inca
	kload8	kreg+10
	kcmpU8	kreg+8
	kjmpnc	ttf@get@20
	ksub8	kreg+9
	kstore8	kreg+10
	kload8	kreg+8
	ksub8	kreg+9
	knorm8	kreg+10
	kl1load	2
	return
ttf@get@20
	kset8	255
	kl1load	2
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
	local	next
	clrf	kreg
	kjmps	be,7,next
	kload8	ksig
	kmulU8xU16 be
	kshlU16	5
next
	kmaxU8	ausg
	kmaxU8	ausl
	kstore8	brkrate
	endm
Adecel0 macro
	local	@010
	kload8	bfp
	kcvsU8
	ksub8	bfn
	kstore8	abfo
	kset8	19
	call	tbl@seta
	kload8	zovr
	call	ttf@get
	kstore8	ansg	;delay cycle
	endm
Adecel1 macro	;get brake
	local	@102,@103,@110,@120
	kset8	14
	call	tbl@seta
	kload8	zovr
	call	ttf@get
	kstore8	ahsg	;tension ref
	kminU8	bfp
	kmulU8xU8 csig
	kstore8	kreg+8
	kload8	ahsg
	ksub8	kreg+8
	kmulU8xU8 aksg
	kshlU16	3
	kstore8	ausg
	call	rb@push
	clrf	kreg+8	;eval time
@103
	kload8	kreg+8
	kcmpU8	ansg
	kjmpc	@110
	incf	kreg,F
	call	rb@pop
	kminU8	ausg
	kstore8	ausg
	incf	kreg+8,F
	goto	@103
@110
	endm
Adecel2 macro
	local	@201,@202,@203,@210
	kload8	zovr
	kcmpU8	tstl
	kjmpnc	@210
	clrf	tpwm
	kjmpns	oflag,1,@210	;no stoll control
	kload8	abfo
	ksub8	hstl
	kstore8	kreg+8
	kload8	ustl
	kmulU8xS8 kreg+8
	kstore16 kreg+8
	kload16 ausl
	call	k8@reduce
	kstore16 ausl
@210
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
	kload8	ansg
exit
	endm
