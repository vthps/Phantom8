program_NAME := hps_racecode# Name of final elf/hex files

program_SRCS := $(wildcard *.cpp) # C++ source files for Race Code
program_OBJS := ${program_SRCS:.cpp=.o} # Change race code C++ files to object files


library_CPP_SRCS := $(wildcard libraries/*.cpp)# C++ source files from included libraries
library_C_SRCS := $(wildcard libraries/*.c)# C source files from included libraries


library_CPP_OBJS := ${library_CPP_SRCS:.cpp=.o} #Change .cpp to .o and strip off libraries/
library_C_OBJS := ${library_C_SRCS:.c=.o} #Change .c to .o and strip off libraries/


CC = avr-gcc #Define compilers
CXX = avr-g++ #Generally a bad practice, but whatever

MCU = atmega2560 # Arduino Mega
F_CPU = 16000000L # 16 Mhz clock speed
ARDUINO = 10609 # Arduino IDE Version
SERIAL_PORT = /dev/tty.usbmodem1411 # Serial port arduino is connected to

CFLAGS := -g -Os -Wall -Wextra -std=gnu11 -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO) -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR
CXXFLAGS := -g -Os -Wall -Wextra -std=gnu++11 -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO) -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR
#CFLAGS += -MMD #Enable make dependency file generation
#CXXFLAGS += -MMD  

LINKFLAGS = -Os -Wl,--gc-sections,--relax

.PHONY: all compile clean viewhex 


all: $(program_NAME).hex
	avr-size -C --mcu=$(MCU) $(program_NAME).elf

compile:
	@echo "************ Compiling source files ************"
	$(CXX) $(CXXFLAGS) -c $(program_SRCS) # Compile C++ files for programs
	$(CXX) $(CXXFLAGS) -c $(library_CPP_SRCS) # Compile C++ files for programs
	$(CC) $(CFLAGS) -c $(library_C_SRCS) # Compile C++ files for programs
	@- echo
	

$(program_NAME).elf: compile
	@echo "************ Linking object files against cores/megaCore.a and libraries/megaLibs.a ************"
	avr-gcc $(LINKFLAGS) -mmcu=$(MCU) $(program_OBJS) $(library_CPP_OBJS:libraries/%=%) $(library_C_OBJS:libraries/%=%) cores/mega_core.a -o $(program_NAME).elf
	mv *.o objects
	@- echo
	
$(program_NAME).hex: $(program_NAME).elf
	@echo "************ Generating HEX file ************"
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(program_NAME).elf $(program_NAME).eep
	avr-objcopy -O ihex -R .eeprom $(program_NAME).elf $(program_NAME).hex
	@- echo

clean:
	@- rm -f *.o
	@- rm -f *.d
	@- rm -f $(program_NAME).elf
	@- rm -f $(program_NAME).eep
	@- rm -f $(program_NAME).hex
	@- rm -f objects/*.o

viewhex: $(program_NAME).hex
	cat $(program_NAME).hex
	@- echo
	
program: all
	avrdude -C cores/avrdude.conf -v -p $(MCU) -c wiring -P $(SERIAL_PORT) -b115200 -D -Uflash:w:$(program_NAME).hex:i
	



