APPNAME = $(shell basename $(PWD))

# The name of your project (used to name the compiled .hex file)
TARGET = main

# set your MCU type here, or make command line `make MCU=MK20DX256`
MCU=MKL26Z64
MCU_LD = $(MCU).ld

# configurable options
OPTIONS = -DF_CPU=48000000 -D__$(MCU)__ -DUSB_SERIAL

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -Werror -mthumb -g -MMD $(OPTIONS) -I./inc -mcpu=cortex-m0plus -nostdlib -fsingle-precision-constant

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections 

ASFLAGS = -x assembler-with-cpp 

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -nostartfiles -Wl,--gc-sections,--no-wchar-size-warning --specs=nano.specs -mcpu=cortex-m0plus -mthumb -T$(MCU_LD)

# additional libraries to link
LIBS = -lm

# names for the compiler programs
CC = $(COMPILERPATH)/arm-none-eabi-gcc
CXX = $(COMPILERPATH)/arm-none-eabi-g++
OBJCOPY = $(COMPILERPATH)/arm-none-eabi-objcopy
SIZE = $(COMPILERPATH)/arm-none-eabi-size

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

# automatically create lists of the sources and objects
C_FILES := $(shell find src/ -type f -name *.c)
S_FILES := $(shell find src/ -type f -name *.S)
CPP_FILES := $(shell find src/ -type f -name *.cpp)

OBJS := $(patsubst src/%.c,obj/%.o,$(C_FILES))  $(patsubst src/%.cpp,obj/%.o,$(CPP_FILES))  $(patsubst src/%.S,obj/%.o,$(S_FILES)) 

# the actual makefile rules (all .o files built by GNU make's default implicit rules)

all: bin/$(TARGET).hex

obj/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

bin/$(TARGET).elf: $(OBJS) $(MCU_LD)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

bin/%.hex: bin/%.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	arm-none-eabi-readelf -a $< > bin/$(TARGET).lst
	$(OBJDUMP) -x -S $< >> bin/$(TARGET).lst
	$(OBJDUMP) -x -j .data $< >> bin/$(TARGET).lst
	$(OBJDUMP) -x -j .bss $< >> bin/$(TARGET).lst
	arm-none-eabi-readelf -x .data $< >> bin/$(TARGET).lst

# compiler generated dependency info
-include $(OBJS:.o=.d)

burn: bin/$(TARGET).hex
	teensy_loader_cli -mmcu=$(MCU) -w -v $<
clean:
	rm -Rf obj/*.o obj/*.d bin/*.elf bin/*.hex bin/*.lst
zip:
	(cd ..; \
        zip -FS -r $(APPNAME) $(APPNAME) \
        -x @'$(APPNAME)/.gitignore' -x '$(APPNAME)/.git/*')

