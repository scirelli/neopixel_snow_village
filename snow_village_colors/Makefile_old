SHELL:=/bin/bash

PP=/home/scirelli/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-g++
CTAGS=/home/scirelli/.arduino15/packages/builtin/tools/ctags/5.8-arduino11/ctags
CC=/home/scirelli/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc
OC=/home/scirelli/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-objcopy
AS=/home/scirelli/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-size

install:
	sudo apt install gcc-avr avr-libc avrdude

build:
	$(PP) -c -g -Os -w -std=gnu++11 -fpermissive \
		-fno-exceptions \
		-ffunction-sections \
		-fdata-sections \
		-fno-threadsafe-statics \
		-Wno-error=narrowing \
		-flto -w -x c++ -E -CC \
		-mmcu=attiny85 -DF_CPU=8000000L -DARDUINO=10607 -DARDUINO_AVR_TRINKET3 \
		-DARDUINO_ARCH_AVR \
		-I/home/scirelli/.arduino15/packages/arduino/hardware/avr/1.8.6/cores/arduino \
		-I/home/scirelli/.arduino15/packages/adafruit/hardware/avr/1.4.15/variants/tiny8 \
		-I/home/scirelli/Arduino/libraries/Adafruit_NeoPixel \
		./main.c \
		-o ./build/sketch_merged.cpp
	$(CTAGS) -u --language-force=c++ -f - \
		--c++-kinds=svpf --fields=KSTtzns \
		--line-directives \
		./build/sketch_merged.cpp
	$(PP) -c -g -Os -w -std=gnu++11 -fpermissive \
		-fno-exceptions \
		-ffunction-sections \
		-fdata-sections \
		-fno-threadsafe-statics \
		-Wno-error=narrowing -MMD \
		-flto -mmcu=attiny85 -DF_CPU=8000000L -DARDUINO=10607 -DARDUINO_AVR_TRINKET3 \
		-DARDUINO_ARCH_AVR \
		-I/home/scirelli/.arduino15/packages/arduino/hardware/avr/1.8.6/cores/arduino \
		-I/home/scirelli/.arduino15/packages/adafruit/hardware/avr/1.4.15/variants/tiny8 \
		-I/home/scirelli/Arduino/libraries/Adafruit_NeoPixel \
		./main.c \
		-o ./build/main.o
	$(CC) -w -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections \
		-mmcu=attiny85 \
		-o ./build/main.elf \
		./build/main.o \
		./build/Adafruit_NeoPixel.cpp.o \
		./build/esp.c.o \
		./build/esp8266.c.o \
		./build/kendyte_k210.c.o \
		./build/core.a \
		-L/home/scirelli/.var/app/cc.arduino.IDE2/cache/arduino/sketches/B36C69AEA7BA79B74E955C5E50F9A51D -lm
	$(OC) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 /home/scirelli/.var/app/cc.arduino.IDE2/cache/arduino/sketches/B36C69AEA7BA79B74E955C5E50F9A51D/snow_village_colors.ino.elf /home/scirelli/.var/app/cc.arduino.IDE2/cache/arduino/sketches/B36C69AEA7BA79B74E955C5E50F9A51D/snow_village_colors.ino.eep
	$(OC) -O ihex -R .eeprom /home/scirelli/.var/app/cc.arduino.IDE2/cache/arduino/sketches/B36C69AEA7BA79B74E955C5E50F9A51D/snow_village_colors.ino.elf /home/scirelli/.var/app/cc.arduino.IDE2/cache/arduino/sketches/B36C69AEA7BA79B74E955C5E50F9A51D/snow_village_colors.ino.hex
	$(AS) -A /home/scirelli/.var/app/cc.arduino.IDE2/cache/arduino/sketches/B36C69AEA7BA79B74E955C5E50F9A51D/snow_village_colors.ino.elf
