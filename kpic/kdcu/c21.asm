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
#include "../krisna/inc"

;File Registers;;;;;;;;;;;;;;;;;;;;;;;;;;
	kmem 0x20
	cblock
	;global params
		oflag,wflag,tacl,tsamp
		tpwm,pole,apfm,tobpi
	;global vars
		cstat	;0-3=accel
		tcount:2,tscan:2,tnext:2,tmsec:2,tstoll
	;tob observer
		B2pi
		we:2,be:2,dbe:2,err:2,k1e,k2e,wtop
	;brake duty control
		brkrate,brkstat,brkduty
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

#ifndef	EE00
#define	EE00	0	;option flag
#endif
#ifndef	EE01
#define	EE01	1	;watch selection
#endif
#ifndef	EE02
#define	EE02	14	;start dt
#endif
#ifndef	EE03
#define	EE03	20	;sampling time
#endif
#ifndef	EE04
#define	EE04	20	;pwm dt
#endif
#ifndef	EE05
#define	EE05	40	;pole
#endif
#ifndef	EE06
#define	EE06	100	;accel brake limit
#endif
#ifndef	EE07
#define	EE07	200	;2Pi 2^5
#endif
;;EE Data;;;;;;;;
;;basic(16 bytes)
	org	0x2100
	de	EE00,EE01,EE02,EE03	;+0
	de	EE04,EE05,EE06,EE07	;+4
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
	de	20,30,45,60,68,85,102,119,136,153,170,187,204,221,238,255
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
	kset8	8
	ke2readn oflag
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
	call		brk@restart
	kjmps	cstat,5,revo@decel.10
	call		brk@timeout
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
	call		brk@pwm
	kload8	brkrate
	kmacns	oflag,0,kspsend
	Uson
;	kset8	240	;94%
	kset8	230	;90%
	kmulU8xU16 tcount
	kstore16 tnext
	kjmpns	oflag,2,revo@loop
revo@decel.47	;sensor
	kt1hold16
	kcmpU16 tnext
	kjmpnc revo@decel.47
	call		brk@halt
	goto		revo@loop
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
	return
revo@wat3
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
	kjmps	brkstat,2,brk@out02
	Ubon
brk@out02
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
	clrf		brkstat
	Uboff
	return
brk@halt
	bsf		brkstat,2
	Uboff
	return
brk@restart
	btfss	brkstat,2	;if halt flag set
	return
	kjmpns 	brkstat,1,brk@restart02
	Ubon
brk@restart02
	bcf		brkstat,2
	return
;@kt0set8.8
;	kmov8	KT0HI,kreg
;	iorwf	kreg+1,W
;	btfsc	STATUS,Z
;	incf		kreg+1,F
;	movfw	kreg+1
;	sublw	0
;	movwf	TMR0
;	btfsc	STATUS,C
;	decf		KT0HI,F
;@kt0set@ret
;	bcf		KT0IR
;	bsf		KT0IE
;	bcf		KT0UP
;	return

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
	kclr24	we
	kclr24	be
	kload8	tobpi
	kexmU8
	knorm16 tcount
	kstore8	we
	return
tob@gain
	kload8	pole
	kshlU8	1
	kstore8	k1e
	kload8	pole
	kmulU8xU8 pole
	kshlU16	4
	kstore8	k2e
	return
tob@renew
	kset24	628*(1<<16)/100
	kstore24 kreg+8
	kload16 tcount
	kmulU16xU16 we
	kcvsU32
	kshrS32 5
	ktrmS32
	knegS24
	kadd24 kreg+8
	ktrmS24
	kstore16 err
	kload8 k1e
	kmulU8xS16 err
	kshrS16 5
	kadd16 we
	kstore16 we
	kload16 tcount
	kmulU16xS16 be
	kshrS24 7
	ktrmS24
	kadd16 we
	kstore16 we
	kload8 B2pi
	kmulU8xU8 brkrate
	kexmU8
	kcvsU16
	knegS16
	kadd16	we
	kstore16 we

	kload8 k2e
	kmulU8xS16 err
	kshrS16	3
	kstore16 dbe
	kadd16	be
	kstore16 be

	return
	end
