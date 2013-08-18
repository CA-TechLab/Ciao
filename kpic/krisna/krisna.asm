#include	"..\krisna\inc"

#include <p16f688.inc>
	__config _CP_OFF & _WDT_OFF & _PWRTE_OFF & _INTOSCIO & _BOD_OFF & _MCLRE_OFF & _IESO_OFF & _FCMEN_OFF
	list r=dec
;memory allocation Krisna
	kmem 0x20
	cblock
		x:4,y:4,z:4
	endc

	org	0x2100
	de	0,10,20,30,40

reset	org	0
;init Krisna
	kinit	8000000
	kt0init	1000
	kt1init	1000
	ke2init
	kspinit 25;19200
	kl1init
	kadinit
;init Peripherals(F688)
	kset8	B'01110001'		;Internal 8MHz
	kstore8	OSCCON
	kset8	B'01110000'		;ADC by internal osc
	kstore8	ADCON1
	kset8	B'11101111'		;enable serial out
	kstore8	TRISC
;user codes
	kset16	10*256
	kt0set
	kt1reset
	nop
loop1
	btfss	KT0UP
	goto	loop1
	kset8	0
	ke2seta
	kset8	128
	nop
	kl1load	5
	nop
	kadstart 0
	nop
	kmulU8xU16 x
	nop
	kmulU16xU16 x
	nop
	kspron
loop9
	nop
	kspcheck
	kcmpL8	0xFF		;no received data
	kjmpz	loop9
	goto	loop9

	end
