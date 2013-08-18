#define	ktmp	kreg+8

	cblock
	madr,mscl
	sret,sga1,sga2,sgb2
	sglo,srem
;vars
	aghi:2,aghp
	agth:2,agtp
	agtn,aghk,agtk
	abe,adbe:2
	endc

#ifdef M443	;D34-W24-H3(BassOne pla-spool)
#define	WSOUND
Adata macro
	org	0x2100+48
	de	6,200,200,200
	de	0,0,0,0
	de	100,100,244,50
	de	60,50,0,0
	de	250,255,230,170
	de	225,1,220,0
	endm
#endif
#ifdef M414	;D34-W21-H4(Exsence pla-spool)
#define	EE00		4	;halt on
Adata macro
	org	0x2100+48
	de	6,230,230,230
	de	230,230,230,230
	de	90,100,244,50
	de	60,50,0,0
	de	250,255,230,170
	de	225,1,220,0
	endm
#endif
#ifdef M022	;D30-W22-H2
#define	EE06	200
#define	WSOUND
Adata macro
	org	0x2100+48
	de	6,120,120,120
	de	0,0,0,0
	de	240,100,244,50
	de	60,50,0,0
	de	250,255,230,170
	de	225,1,220,0
	endm
#endif
#ifdef M4571	;D34-W25-H7(Scorpion) Type-I
#define	MOPTION	4	;halt on
#define	WSOUND
Adata macro
	org	0x2100+48
	de	4,170,170,170
	de	32,64,96,255
	de	120,140,80,60
	de	60,80,0,0
	de	255,255,150,100	;brake table
	de	70,50,40,30
	endm
#endif

mdata	macro	ofs
	kset8	ofs
	call		tbl@read
	endm

Aset macro
	local	@10,@20,@21,@22,@100
	kstor8	ktmp	;mode
	kset8	48
@10
	kstor8	madr
	ke2seta
	ke2read
	kcmpU8	ktmp
	kjmpc	@20
	kset8	24
	kadd8	madr
	goto		@10
@20
	ksub8	ktmp
	ke2adda
	ke2inca
	ke2read
	kstor8	mscl
;	kstor8	B2pi	;test
#ifdef	WSOUND
	kset8	3
	ke2adda
	ke2read
	kstor8	abe	;use as sound threshold
#endif
	kset8	8
	call		tbl@seta
	kset8	8
	ke2readn sret
	kmov8	B2pi,sret
;	kmov8	mscl,zovr ;test
	kjmpns	mscl,0,@22
	bsf		oflag,1	;stoll control on
@22
	kset8	255
	kstor8	aghi
	kstor8	aghi+1
	kstor8	aghp
	kstor8	agth
	kstor8	agth+1
	kstor8	agtp
	clrf		agtn
	call		ag@load
	goto		@100
tbl@seta
	kadd8	madr
	ke2seta
	return
tbl@read
	call		tbl@seta
	ke2read
	return
ag@load
	tstf		agtn
	kjmpz	ag@load@next
	tstf		agtk
	kjmpz	ag@load@set	;if agtk is 0,end of the table
ag@load@next
	kset8	16
	kadd8	agtn
	call		tbl@seta
	kset8	2
	ke2readn aghk
	kset8	2
	kadd8	agtn
	kstor8	agtn
ag@load@set
	kload8	aghi+1
	kstor8	aghi
	kmulU8xU8 aghk
	kstor8	aghi+1
	kload8	agth+1
	kstor8	agth
	kmulU8xU8 agtk
	kstor8	agth+1
	return
@100
	endm
Aaccel	macro
	kset8	255
	kstor8	brkrate
	endm
Atop		macro
	local	exit
	kjmps	oflag,1,exit	;stoll control on
#ifdef	WSOUND
	kload8	we
	kcmpU8	abe
	kjmpnc	exit
#endif
	clrf		tpwm	;sound on
exit
	clrf		sret
	kload8	srem
	kmulU8xU8 wtop
	kshlU16 2
	kstor8	srem
	endm
Ascan	macro
	local	@abe,@ip,@smc,@smc1,@lo,@mlo,@hi,@out,@exit
	incf		sret,F
@abe
	kload16	be
	kshlS16	3
	kstor8	abe
	kload8	we
	kmulU8xS16 dbe
	kshrS24	3
	ktrmS24
	kstore16	adbe
@smc
	kload8	sga1
	kabsS8
	kmulU8xS8 abe
	kjmpns	sga1,7,@smc1
	knegS16
@smc1
	kadd16	adbe
	kstor16	ktmp

	kload8	abe
	ksub8	sgb2
	kstore8	ktmp+1
	kload8	sga2
	kabsS8
	kmulU8xS8 ktmp+1
	kjmpns	sga2,7,@smc2
	knegS16
@smc2
	kadd16	adbe
	kstor8	ktmp+1

	kjmps	ktmp,7,@hi
	kjmps	ktmp+1,7,@hi
@lo
	tstf		agtp
	kjmpnz	@mlo
	kload8	sglo
	kmulU8xU8 aghp
	goto		@out
@mlo
	kset8	255
	ksub8	sglo
	kstor8	ktmp
	kload8	abe
	kcvsS8
	knorm8	agtp
	kmulU8xU8 ktmp
	kstor8	ktmp
	kset8	255
	ksub8	ktmp
	kmulU8xU8 aghp
	goto		@out
@hi
	kload8	sret
	knorm8	srem
	kstor8	ktmp
	kload16	aghi
	movfw	ktmp
	call		@kl1ip
	kstor8	aghp
	kload16	agth
	movfw	ktmp
	call		@kl1ip
	kmulU8xU8 sgb2
	kstor8	agtp
	kload8	aghp
	goto		@out
@out
	kmulU8xU8 mscl
	kstor8	brkrate
@exit
	endm
Adecel0 macro
	local	@exit
	kload8	sret
	kcmpU8	srem
	kjmpnc	@exit
	call		ag@load
	clrf		sret
@exit
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
	kload8	agtp
	goto		exit
odd
exit
	endm
