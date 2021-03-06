	list r=dec

#ifdef	K15
#include "K15.unit"
#endif
#ifdef T08
#include "T08.unit"
#endif
#ifdef T15
#include "T15.unit"
#endif

#define	KT0HOOK brk@out
#define	KSPHOOK1 ssl@diag
#define	KSPHOOK2 sens@diag
#include "..\krisna\inc"

;File Registers;;;;;;;;;;;;;;;;;;;;;;;;;;
	kmem 0x20
	cblock
	;global params
		oflag,wflag,tacl,tsamp
		tpwm,pole:2,apfm
		tobpi
	;global vars
		cstat	;0-3=accel
		tcount:2,tscan:2,tnext:2,tmsec:2,tstoll
	;tob observer
		B2pi
		we:3,be:3,err:2,k1e,k2e,pi2:3
		bfu,bfa:2,bfp,bfn,dbf,wtop
	;brake duty control
		brkrate,brkstat,brkduty,brkpc:2
	;rotation sensor
		senstat
	;dial
		sslstat
	endc
;;user registers
#if ALGOR==0
#include "A0.inc"
#endif
#if ALGOR==1
#include "A1.inc"
#endif
#if ALGOR==2
#include "A2.inc"
#endif
#if ALGOR==3
#include "A3.inc"
#endif
#if ALGOR==4
#include "A4.inc"
#endif
#if ALGOR==5
#include "A5.inc"
#endif
#if ALGOR==6
#include "A6.inc"
#endif
#if ALGOR==7
#include "A7.inc"
#endif
#if ALGOR==8	;Scorpion
#include "A8.inc"
#endif

;;EE Data;;;;;;;;
;;basic(16 bytes)
	org	0x2100
#ifndef Mbasic
	de	0,3,14,20	;+0
	de	20,40,20,200	;+4
	de	200		;+8 tobpi=6.28*32
#else
	Mbasic
#endif
;;select switch(8 bytes)
	org	0x2100+12
#ifndef	Ussl
	de	0,0,0,0		;accuracy,default mode
	de	20,40,60,80	;ch0,ch1...
#else
	Ussl
#endif
;;Test mode output
	org	0x2100+40
#ifndef	Utest
	de	20,40,60
#else
	Utest
#endif
;;J and Duty Calibration Area(1+15 bytes)
	org	0x2100+24
	de	0,30,45,60,68,85,102,119,136,153,170,187,204,221,238,255
;#include "calib.inc"

;;User Params
	org	0x2100+48
	Adata

	org		0
reset
	kinit	8000000	;8MHz
	kt0init	1000
	kt1init	1000
	ke2init
	kspinit 25;19200
	kadinit
	kl1init
;init Peripherals
init
	Uinit
	banksel	TMR0
	call	brk@init
	call	sens@init
	call	ssl@init
;;Wait Mode;;;;;;;;;;;;;;;;;;
wait
	kset8	0
	ke2seta
	kset8	9
	ke2readn oflag			;loading 7 byte onto oflag...
wait@retry
	clrf	cstat
	Uson
	kt1reset
	kspron
wait@loop
	kspcheck
	kcmpL8	0xff
	kjmpnz	wait
	kt1hold8
	kshlU8	1	;*2
	kcmpU8	tacl
	kjmpc	wait@retry
	call	sens@edge
	kjmpnc	wait@loop
	kmov16	kreg+8,tcount
	kt1hold16
	kt1reset
	kstore16 tcount
	incf	cstat,F
	kload8	cstat
	kcmpL8	3
	kjmpc	wait@next
	goto	wait@loop
wait@next
	kload16	tcount
	kadd16	kreg+8
	kstore16 tcount

;;Revo Event;;;;;;;;;;
revo
	call	tob@init
	call	brk@init
	clrf	cstat
	kclr16	tmsec
	kmov8	tstoll,tsamp
	kset16	65535
	kstore16 tscan
	ksproff
	kset8	0xE0	;N data Transmission from debug register
	kspsend
	kset8	16		;address of debug register
	btfss	wflag,0 
	incf	kreg,F
	kspsend
	call	ssl@decode
	kjmpnz	revo@sslok
	call	ssl@decode
	kjmpnz	revo@sslok
	kset8	13	;default dial
	ke2seta
	ke2read
revo@sslok
	kspsend
	kstore8	sslstat
	Aset
revo@loop
	kt1hold8
	kcmpU8	tstoll
	kjmpc	revo@next
	call	sens@edge
	kjmpnc	revo@loop
	kjmpns	cstat,7,revo@check
	kt1hold16
	kt1reset
	kadd16	tnext
	goto	revo@break
revo@check
	kt1hold16
	kt1reset
	kstore16 tnext
	ksub16 tcount
	kabsS16
	kstore16 kreg+8
	kload16 tnext
	kshlU16 1
	ksub16 tcount
	kabsS16
	kcmpU16 kreg+8
	kload16 tnext
	kjmpc	revo@break
	bsf	cstat,7	;half turn flag
	goto	revo@loop
revo@break
	kstore16 tcount
	kshlU16	3
	kmacns oflag,0,kspsend
	bcf	cstat,7
	kload8	tcount
	kexmU8
	kadd16	tmsec
	kstore16 tmsec
	kjmps	cstat,6,revo@decel
revo@accel
	call	brk@timeout
	call	tob@init
	call	revo@wat
	kmacns oflag,0,kspsend
	kmov8	wtop,we
	call	revo@Maccel
	kload8	brkrate
	kminU8	apfm
	kstore8	brkrate
	kmacns oflag,0,kspsend
	goto	revo@loop
revo@Maccel
	kload16 tmsec
	kcmpL16	500
	kjmpc	revo@Maccel8
	kload16 tcount
	kcmpU16	tscan
	kjmpc	revo@Maccel7
	kstore16 tscan	;min tcount
	clrf	cstat
revo@Maccel1
	Aaccel
	return
revo@Maccel7
	incf	cstat,F
	kload8	cstat
	kcmpL8	3
	kjmpnc	revo@Maccel1
revo@Maccel8
	clrf	cstat
	bsf	cstat,6	;decel mode
	kclr16	tscan
	Atop
	return
revo@decel
	Usoff
	kjmps	cstat,5,revo@decel.10
	call	brk@timeout
	kload8	tcount
	kcmpU8	tpwm
	kjmpnc	revo@decel.10
	bsf		cstat,5
revo@decel.10
	kset16	2*256	;2msec
	kminU16	tcount
	knegU16
	kadd16	tcount
	kstore16 tnext	;next timer
	kload16	tscan
	kadd16	tcount
	kstore16 tscan
	call	tob@gain
	call	tob@renew
	call	revo@wat
	kmacns oflag,0,kspsend
	Ascan
	kload8	tscan
	kcmpU8	tsamp
	kjmpnc	revo@decel.30
	ksub8	tsamp
	kstore16 tscan
	movlw	0xf0
	andwf	cstat,F
	call	tob@bf
revo@decel.30
#ifndef	TSTSKIP
	kload16	tmsec
	kcmpL16	1000
	kjmpc	revo@decel.40
	kload8	wtop
	ksub8	we
	kabsS8
	kcmpL8	300/32
	btfsc	STATUS,C
	bsf	cstat,4
	goto	revo@decel.41
revo@decel.40		;>1500msec
	kjmps	cstat,4,revo@decel.41
	kload8	sslstat
	call	revo@test
	goto	revo@decel.42
#endif
revo@decel.41	;M-control
	call	revo@Mdecel
	kt1hold16
	kcmpU16 tnext
	kjmpnc revo@decel.41
revo@decel.42
	btfsc	cstat,5
	call	brk@pwm
	kload8	brkrate
	kmacns oflag,0,kspsend
	Uson
	kt1hold16
	kstore16 tnext
	kset16	256*50/1000 ;50usec
	kadd16 tnext
	kstore16 tnext
revo@decel.47	;sensor
	kt1hold16
	kcmpU16 tnext
	kjmpnc revo@decel.47
	goto	revo@loop
revo@Mdecel
	movlw	0x0f
	andwf	cstat,W
	kjmptbl
	goto	revo@Mdecel0
	goto	revo@Mdecel1
	goto	revo@Mdecel2
	goto	revo@Mdecel3
	goto	revo@Mdecel4
revo@Mdecel0
	Adecel0
	incf	cstat,F
	return
revo@Mdecel1
	Adecel1
	incf	cstat,F
	return
revo@Mdecel2
	Adecel2
	incf	cstat,F
	return
revo@Mdecel3
	Adecel3
	incf	cstat,F
	return
revo@Mdecel4
	return
revo@test
#ifndef	__16F688
;	call	ssl@decode	;kreg+8=AD
;	kjmpnz	revo@test.ok
;	kset8	1
;	kstore8	brkrate
;	return
;revo@test.ok
	ke2seta
	kset8	40
	ke2adda
	ke2read
	kstore8	brkrate
#endif
	return
revo@wat
	kload8	wflag
	kcmpL8	4
	kjmpc	revo@wat4
	movfw	kreg
	kjmptbl
	goto	revo@wat0
	goto	revo@wat1
	goto	revo@wat2
	goto	revo@wat3
revo@wat0
	kload8	we
	return
revo@wat1
	kload16	be
	kshlS16 3
	return
revo@wat2
	kload8	bfp
	return
revo@wat3
	kload8	bfp
	kcvsU8
	ksub8	bfn
	return
revo@wat4
	Awat
	return
revo@next

;;spool stoll Mode;;;;;;;;;;
stoll
	Usoff
	call	brk@stop
#ifndef	DEVEL
	kset8	B'01000001';clock down to 1MHz
	kstore8	OSCCON
#else
	banksel	RCSTA
	bcf	RCSTA,SPEN
	banksel TXSTA
	bcf	TXSTA,TXEN
	banksel	TRISC
	bsf	TRISC,4
	banksel	TMR0
#endif
	kclr16	tcount
	kload8	brkrate
	kjmpns	oflag,1,stoll@ini
	kset8	255*9/10
stoll@ini
#ifndef	DEVEL
	kshrU8	3
#endif
	kstore8	brkrate
	Ubon
stoll@loop
	kload16	tcount
	kinc16
	kstore16 tcount
	kcmpL8	3
	kjmpns	oflag,1,stoll@ord
	kcmpL8	10
stoll@ord
	kjmpc	reset
stoll@woff
	kload8	TMR0
#ifndef	DEVEL
	movlw	0x1f
	andwf	kreg,F
#endif
	kcmpU8	brkrate
	kjmpnc	stoll@woff
	Uboff
stoll@won
	kload8	TMR0
#ifndef	DEVEL
	movlw	0x1f
	andwf	kreg,F
#endif
	kcmpU8	brkrate
	kjmpc	stoll@won
	Ubon
	goto	stoll@loop
	
;;subroutines;;;;;;;;;;;;;;;
;1)FET control
brk@init
	clrf	brkrate
	clrf	brkduty
	goto	brk@stop
brk@pwm
	call	brk@cal
	kstore8	brkduty
	btfsc	brkstat,0
	return
	tstf	brkduty
	btfsc	STATUS,Z
	return
	Ubon
	bsf		brkstat,0
	bsf		brkstat,1
	kshrU16 8
	kt0set
	return
brk@timeout
	call	brk@cal
	kmulU8xU16 tcount
	kt0set
	Ubon
	clrf	brkstat
	return
brk@cal
	kset8 24
	ke2seta
	kload8	brkrate
	kl1load 16
	return
brk@out		;interrupt handle
	kjmpns	brkstat,0,brk@stop
	kjmps	brkstat,1,brk@off
	bsf		brkstat,1
	Ubon
	incf	brkpc+1,F
	btfsc	STATUS,Z
	incf	brkpc,F
	movfw	brkduty
	sublw	255
	movwf	TMR0
	bsf		KT0IE
	return
brk@off
	bcf		brkstat,1
	Uboff
	movfw	brkduty
	movwf	TMR0
	bsf		KT0IE
	return
brk@stop
	clrf	brkstat
	Uboff
	return
;2)Sensor control
sens@init
	Usoff
	return
sens@edge	;set carry if up edge found
	Usin
	rrf		senstat,F
	kjmpc	sens@hi
	bcf		senstat,0
	movlw	0xE0
	subwf	senstat,W
	btfsc	STATUS,C
	bsf		senstat,0
	return
sens@hi
	bsf		senstat,0
	movlw	16
	subwf	senstat,W
	btfss	STATUS,C
	bcf		senstat,0
	clrc
	return
sens@diag
	Usin
	clrf	kreg
	rlf		kreg,F
	kspsend
	kset8	11
	kspsend
	kspsend
	return
;3)Select Switch
ssl@init
	return
ssl@decode
#ifndef	SSLSKIP
	Uadcon0
	kexmU8
	kstore16 kreg+8		;AD value
	kset8	12
	ke2seta
	ke2read
	kstore8 kreg+10		;diff
	kset8	14
	ke2seta
	clrf	kreg+11		;dial
ssl@decode.loop
	ke2read
	kcmpL8	255
	kjmpz	ssl@decode.err
	kexmU8
	ksub16	kreg+8
	ktrmS16
	kabsS8
	kcmpU8	kreg+10
	kjmpnc	ssl@decode.ok
ssl@decode.continue
	incf	kreg+11,F
	ke2inca
	goto	ssl@decode.loop
ssl@decode.ok
	movfw	kreg+11
	movwf	kreg
	clrz
	return
ssl@decode.err
	return
#else
	kset8	SSLSKIP
	clrz
	return
#endif
ssl@diag
	call	ssl@decode
	kspsend
	kload8	kreg+8	;AD value
	kspsend
	kload8	kreg+9
	kspsend
	return

;4)Tension observer
tob@init
	kset24	628*(1<<16)/100
	kstore24 pi2
	kclr16	bfa
	clrf	bfu
	clrf	dbf
	clrf	bfp
	clrf	bfn
	kclr24	we
	kclr24	be
	kload8	tobpi
	kexmU8
	knorm16 tcount
	kstore8	we
	return
tob@gain
	kload8	we
	kminU8	pole
	kmaxU8	pole+1
	kstore8	k2e
	kshlU8 1
	kstore8 k1e
	kload8	k2e
	kmulU8xU8 k2e
	kshlU16 4
	kstore8	k2e
	return
tob@renew
	kload16 tcount
	kmulU16xU16 we
	kcvsU32
	kshrS32 5
	ktrmS32
	knegS24
	kadd24 pi2
	ktrmS24
	kstore16 err
	kload8 k1e
	kmulU8xS16 err
	kshrS24 5
	kadd24 we
	kstore24 we
	kload16 tcount
	kmulU16xS16 be
	kshrS32 7
	ktrmS32
	kadd24 we
	kstore24 we
	kload8 B2pi
	kmulU8xU8 brkrate
	kexmU16
	kcvsU24
	knegS24
	kadd24	we
	kstore24 we	;renew we

	kload8 k2e
	kmulU8xS16 err
	kshrS24 3
	kadd24 be
	kstore24 be

	incf	bfu,F
	kload16 be
	kshrS16 2
	kstore16 kreg+8
	kload16 bfa
	kintegS16 kreg+8
	kstore16 bfa
	return
tob@bf
	kload8	bfp
	ksub8	bfn
	kstore8	dbf
	kload8	bfu
	kshlU8	3
	kstore16 kreg+8
	kload16	bfa
	kjmps	bfa,7,tob@bf.n
	knorm16 kreg+8
	kstore8 bfp
	clrf	bfn
	goto	tob@bf.x
tob@bf.n
	knegS16
	knorm16 kreg+8
	kstore8 bfn
	clrf	bfp
tob@bf.x
	clrf	bfu
	kclr16	bfa
	kload8	bfp
	ksub8	bfn
	ksub8	dbf
	kstore8	dbf
	return
	end
