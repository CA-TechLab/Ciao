target:
	-rm -f *.cod *.lst *.hex
	gpasm -p12F683 -DALGOR=1 -DT15 -DM414 -o414.hex c21.asm
	gpasm -p16F688 -DALGOR=1 -DK15 -DM414 -DTSTSKIP -o414k.hex c21.asm
#BassOne
	gpasm -p12F683 -DALGOR=1 -DT15 -DM443 -DSSLSKIP=6 -o443r6.hex c21.asm
#Cardiff
	gpasm -p12F683 -DALGOR=1 -DT15 -DM022 -DSSLSKIP=6 -o022r6.hex c21.asm
#Scorpion w-Sound
#	gpasm -p12F683 -DALGOR=1 -DT15 -DM4571 -o4571.hex c19.asm
