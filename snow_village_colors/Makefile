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
AVR_GPP = $(AVR_GCC_PATH)/avr-g++
AVR_GCC = $(AVR_GCC_PATH)/avr-gcc
AVR_AR = $(AVR_GCC_PATH)/avr-gcc-ar
AVR_OBJCOPY = $(AVR_GCC_PATH)/avr-objcopy

# Arduino core paths
ARDUINO_HARDWARE_PATH = /home/scirelli/.arduino15/packages/arduino/hardware/avr/1.8.6
ADAFRUIT_HARDWARE_PATH = /home/scirelli/.arduino15/packages/adafruit/hardware/avr/1.4.15

# Core and variant paths
CORE_PATH = $(ARDUINO_HARDWARE_PATH)/cores/arduino
VARIANT_PATH = $(ADAFRUIT_HARDWARE_PATH)/variants/tiny8

# Library paths
USER_LIBRARY_PATH = /home/scirelli/Arduino/libraries
LIBRARIES = Adafruit_NeoPixel

# Source files
SKETCH_INO = snow_village_colors.ino
SKETCH_CPP = $(BUILD_DIR)/sketch/$(SKETCH_INO:.ino=.cpp)

# Object directories
BUILD_DIR = build
CORE_BUILD_DIR = $(BUILD_DIR)/core
LIBRARIES_BUILD_DIR = $(BUILD_DIR)/libraries
SKETCH_BUILD_DIR = $(BUILD_DIR)/sketch

# Objects
CORE_CPP_SOURCES = $(wildcard $(CORE_PATH)/*.cpp)
CORE_C_SOURCES = $(wildcard $(CORE_PATH)/*.c)
CORE_S_SOURCES = $(wildcard $(CORE_PATH)/*.S)
CORE_CPP_OBJECTS = $(patsubst $(CORE_PATH)/%.cpp, $(CORE_BUILD_DIR)/%.cpp.o, $(CORE_CPP_SOURCES))
CORE_C_OBJECTS = $(patsubst $(CORE_PATH)/%.c, $(CORE_BUILD_DIR)/%.c.o, $(CORE_C_SOURCES))
CORE_S_OBJECTS = $(patsubst $(CORE_PATH)/%.S, $(CORE_BUILD_DIR)/%.S.o, $(CORE_S_SOURCES))
CORE_OBJECTS = $(CORE_CPP_OBJECTS) $(CORE_C_OBJECTS) $(CORE_S_OBJECTS)

LIBRARY_Adafruit_NeoPixel_SOURCES = $(wildcard $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/*.cpp) $(wildcard $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/*.c)
LIBRARY_Adafruit_NeoPixel_CPP_OBJECTS = $(patsubst $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/%.cpp, $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel/%.cpp.o, $(filter %.cpp, $(LIBRARY_Adafruit_NeoPixel_SOURCES)))
LIBRARY_Adafruit_NeoPixel_C_OBJECTS = $(patsubst $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/%.c, $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel/%.c.o, $(filter %.c, $(LIBRARY_Adafruit_NeoPixel_SOURCES)))
LIBRARY_Adafruit_NeoPixel_OBJECTS = $(LIBRARY_Adafruit_NeoPixel_CPP_OBJECTS) $(LIBRARY_Adafruit_NeoPixel_C_OBJECTS)
LIBRARY_Adafruit_NeoPixel_A = $(LIBRARIES_BUILD_DIR)/libAdafruit_NeoPixel.a

SKETCH_OBJECT = $(BUILD_DIR)/sketch/$(SKETCH_INO:.ino=.cpp.o)

# Flags
COMMON_FLAGS = -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO_VERSION) -DARDUINO_AVR_TRINKET3 -DARDUINO_ARCH_AVR
CPPFLAGS = $(COMMON_FLAGS)
CFLAGS = $(COMMON_FLAGS) -std=gnu11 -ffunction-sections -fdata-sections -flto -fno-fat-lto-objects
ASFLAGS = -c -g -x assembler-with-cpp -flto -MMD -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO_VERSION) -DARDUINO_AVR_TRINKET3 -DARDUINO_ARCH_AVR

# Include paths
INCLUDE_DIRS = -I$(CORE_PATH) -I$(VARIANT_PATH) -I$(USER_LIBRARY_PATH)/Adafruit_NeoPixel

# Target file
TARGET = $(BUILD_DIR)/$(PROJECT_NAME)

all: $(TARGET).hex

# Create build directories
$(BUILD_DIR)/sketch $(CORE_BUILD_DIR) $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel:
	@mkdir -p $@

# Preprocess sketch (mimicking Arduino IDE)
$(SKETCH_CPP): $(SKETCH_INO) | $(BUILD_DIR)/sketch
	$(AVR_GPP) -w -x c++ -E -CC -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO_VERSION) -DARDUINO_AVR_TRINKET3 -DARDUINO_ARCH_AVR -I$(CORE_PATH) -I$(VARIANT_PATH) -I$(USER_LIBRARY_PATH)/Adafruit_NeoPixel $< -o $@

# Compile sketch
$(SKETCH_OBJECT): $(SKETCH_CPP) | $(BUILD_DIR)/sketch
	$(AVR_GPP) $(CPPFLAGS) $(INCLUDE_DIRS) -x c++ $< -o $@

# Compile core
$(CORE_BUILD_DIR)/%.cpp.o: $(CORE_PATH)/%.cpp | $(CORE_BUILD_DIR)
	$(AVR_GPP) $(CPPFLAGS) $(INCLUDE_DIRS) $< -o $@

$(CORE_BUILD_DIR)/%.c.o: $(CORE_PATH)/%.c | $(CORE_BUILD_DIR)
	$(AVR_GCC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@

$(CORE_BUILD_DIR)/%.S.o: $(CORE_PATH)/%.S | $(CORE_BUILD_DIR)
	$(AVR_GCC) $(ASFLAGS) $(INCLUDE_DIRS) $< -o $@

$(CORE_BUILD_DIR)/core.a: $(CORE_OBJECTS) | $(CORE_BUILD_DIR)
	$(AVR_AR) rcs $@ $^

# Compile Adafruit NeoPixel library
$(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel/%.cpp.o: $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/%.cpp | $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel
	$(AVR_GPP) $(CPPFLAGS) $(INCLUDE_DIRS) $< -o $@

$(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel/%.c.o: $(USER_LIBRARY_PATH)/Adafruit_NeoPixel/%.c | $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel
	$(AVR_GCC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@

$(LIBRARY_Adafruit_NeoPixel_A): $(LIBRARY_Adafruit_NeoPixel_OBJECTS) | $(LIBRARIES_BUILD_DIR)/Adafruit_NeoPixel
	$(AVR_AR) rcs $@ $^

# Link everything
$(TARGET).elf: $(SKETCH_OBJECT) $(CORE_BUILD_DIR)/core.a $(LIBRARY_Adafruit_NeoPixel_A)
	$(AVR_GPP) -Os -flto -Wl,--gc-sections -mmcu=$(MCU) $(SKETCH_OBJECT) $(CORE_BUILD_DIR)/core.a $(LIBRARY_Adafruit_NeoPixel_A) -o $@

# Convert to hex
$(TARGET).hex: $(TARGET).elf
	$(AVR_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $@.eep
	$(AVR_OBJCOPY) -O ihex -R .eeprom $< $@

# Check size
size: $(TARGET).elf
	$(AVR_GCC_PATH)/avr-size -C --mcu attiny85 $<

# Clean
clean:
	rm -rf $(BUILD_DIR)
