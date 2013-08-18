rem clean
del *.cod *.lst *.hex

rem Exsense normal
rem gpasm -p12F683 -DALGOR=0 -DT15 -DM427 -o427T15R.hex c19.asm
rem gpasm -p12F683 -DALGOR=0 -DT15 -DLEFT -DM427 -o427T15L.hex c19.asm

rem Exsense Pla-Spool
gpasm -p12F683 -DALGOR=1 -DT15 -DM414 -o414.hex c21.asm
gpasm -p16F688 -DALGOR=1 -DK15 -DM414 -DTSTSKIP -o414k.hex c21.asm

rem BassOne
rem gpasm -p12F683 -DALGOR=1 -DT15 -DM443 -DSSLSKIP=6 -o443r6.hex c20.asm

rem Scorpion w-Sound
rem gpasm -p12F683 -DALGOR=1 -DT15 -DM4571 -o4571.hex c19.asm

rem Cardiff
rem gpasm -p12F683 -DALGOR=1 -DT15 -DM022 -DSSLSKIP=6 -o022r6.hex c20.asm

pause
