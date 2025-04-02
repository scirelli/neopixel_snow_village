# Project settings
PROJECT_NAME = snow_village_colors
MCU = attiny85
F_CPU = 8000000L
ARDUINO_VERSION = 10607
BOARD = trinket3
ARCH = AVR

# Toolchain paths
ARDUINO_TOOLS_PATH = /home/scirelli/.arduino15/packages/arduino/tools
AVR_GCC_VERSION = 7.3.0-atmel3.6.1-arduino7
AVR_GCC_PATH = $(ARDUINO_TOOLS_PATH)/avr-gcc/$(AVR_GCC_VERSION)/bin
AVR_GPP = $(AVR_GCC_PATH)/avr-g++  # Variable for C++ compiler
AVR_GCC = $(AVR_GCC_PATH)/avr-gcc    # Variable for C compiler
AVR_AR = $(AVR_GCC_PATH)/avr-gcc-ar
AVR_OBJCOPY = $(AVR_GCC_PATH)/avr-objcopy

# Arduino core paths
ARDUINO_HARDWARE_PATH = /home/scirelli/.arduino15/packages/arduino/hardware/avr/1.8.6
ADAFRUIT_HARDWARE_PATH = /home/scirelli/.arduino15/packages/adafruit/hardware/avr/1.4.15

# Core and variant flags
CORE_PATH = $(ARDUINO_HARDWARE_PATH)/cores/arduino
VARIANT_PATH = $(ADAFRUIT_HARDWARE_PATH)/variants/tiny8

# Library paths
USER_LIBRARY_PATH = /home/scirelli/Arduino/libraries
LIBRARIES = Adafruit_NeoPixel

# Source files
SKETCH_INO = snow_village_colors.ino
SKETCH_CPP = $(SKETCH_INO:.ino=.cpp)
CORE_FILES = $(wildcard $(CORE_PATH)/*.cpp) $(wildcard $(CORE_PATH)/*.c) $(wildcard $(CORE_PATH)/*.S)

# Object directories
BUILD_DIR = build
CORE_BUILD_DIR = $(BUILD_DIR)/core
LIBRARIES_BUILD_DIR = $(BUILD_DIR)/libraries
SKETCH_BUILD_DIR = $(BUILD_DIR)/sketch

# Objects
CORE_CPP_OBJECTS = $(patsubst $(CORE_PATH)/%.cpp, $(CORE_BUILD_DIR)/%.cpp.o, $(filter %.cpp, $(CORE_FILES)))
CORE_C_OBJECTS = $(patsubst $(CORE_PATH)/%.c, $(CORE_BUILD_DIR)/%.c.o, $(filter %.c, $(CORE_FILES)))
CORE_S_OBJECTS = $(patsubst $(CORE_PATH)/%.S, $(CORE_BUILD_DIR)/%.S.o, $(filter %.S, $(CORE_FILES)))
CORE_OBJECTS = $(CORE_CPP_OBJECTS) $(CORE_C_OBJECTS) $(CORE_S_OBJECTS)

LIBRARY_FILES_Adafruit_NeoPixel = $(wildcard $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/*.cpp) $(wildcard $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/*.c)
LIBRARY_CPP_OBJECTS_Adafruit_NeoPixel = $(patsubst $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/%.cpp, $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel/%.cpp.o, $(filter %.cpp, $(LIBRARY_FILES_Adafruit_NeoPixel)))
LIBRARY_C_OBJECTS_Adafruit_NeoPixel = $(patsubst $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/%.c, $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel/%.c.o, $(filter %.c, $(LIBRARY_FILES_Adafruit_NeoPixel)))
LIBRARY_OBJECTS_Adafruit_NeoPixel = $(LIBRARY_CPP_OBJECTS_Adafruit_NeoPixel) $(LIBRARY_C_OBJECTS_Adafruit_NeoPixel)

SKETCH_OBJECT = $(SKETCH_BUILD_DIR)/$(SKETCH_CPP:.cpp=.cpp.o)

# Flags
COMMON_FLAGS = -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -flto -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO_VERSION) -DARDUINO_AVR_$(BOARD) -DARDUINO_ARCH_$(ARCH)
CPPFLAGS = $(COMMON_FLAGS) -MMD
CFLAGS = $(COMMON_FLAGS) -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects
ASFLAGS = -c -g -x assembler-with-cpp -flto -MMD -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO_VERSION) -DARDUINO_AVR_$(BOARD) -DARDUINO_ARCH_$(ARCH)

# Include paths
INCLUDE_DIRS = -I$(CORE_PATH) -I$(VARIANT_PATH) -I$(USER_LIBRARY_PATH)/Adafruit_NeoPixel

# Target file
TARGET = $(BUILD_DIR)/$(PROJECT_NAME)

all: $(TARGET).hex

# Core compilation
$(CORE_BUILD_DIR)/%.cpp.o: $(CORE_PATH)/%.cpp
	@mkdir -p $(CORE_BUILD_DIR)
	$(AVR_GPP) $(CPPFLAGS) $(INCLUDE_DIRS) $< -o $@

$(CORE_BUILD_DIR)/%.c.o: $(CORE_PATH)/%.c
	@mkdir -p $(CORE_BUILD_DIR)
	$(AVR_GCC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@

$(CORE_BUILD_DIR)/%.S.o: $(CORE_PATH)/%.S
	@mkdir -p $(CORE_BUILD_DIR)
	$(AVR_GCC) $(ASFLAGS) $(INCLUDE_DIRS) $< -o $@

$(CORE_BUILD_DIR)/core.a: $(CORE_OBJECTS)
	@mkdir -p $(CORE_BUILD_DIR)
	$(AVR_AR) rcs $@ $^

# Library compilation
$(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel/%.cpp.o: $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/%.cpp
	@mkdir -p $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel
	$(AVR_GPP) $(CPPFLAGS) $(INCLUDE_DIRS) $< -o $@

$(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel/%.c.o: $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/%.c
	@mkdir -p $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel
	$(AVR_GCC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@

LIBRARY_Adafruit_NeoPixel_A = $(LIBRARIES_BUILD_DIR)/libAdafruit_NeoPixel.a
$(LIBRARY_Adafruit_NeoPixel_A): $(LIBRARY_OBJECTS_Adafruit_NeoPixel)
	$(AVR_AR) rcs $@ $^

# Sketch compilation
$(SKETCH_OBJECT): $(SKETCH_INO)
	@mkdir -p $(SKETCH_BUILD_DIR)
	$(AVR_GPP) $(CPPFLAGS) $< -o $@

# Linking
$(TARGET).elf: $(SKETCH_OBJECT) $(CORE_BUILD_DIR)/core.a $(LIBRARY_Adafruit_NeoPixel_A)
	$(AVR_GPP) -Os -flto -Wl,--gc-sections -mmcu=$(MCU) $(SKETCH_OBJECT) $(CORE_BUILD_DIR)/core.a $(LIBRARY_Adafruit_NeoPixel_A) -o $@

# Convert to hex
$(TARGET).hex: $(TARGET).elf
	$(AVR_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $@.eep
	$(AVR_OBJCOPY) -O ihex -R .eeprom $< $@

# Clean
clean:
	rm -rf $(BUILD_DIR)
