####################################################################
# Definitions                                                      #
####################################################################

DEVICE      = EFM32G890F128
CPU         = cortex-m3
PROJECTNAME = Controller


####################################################################
# User directories to be included                                  #
####################################################################

USERINCLUDEPATHS = src


####################################################################
# User source files                                                #
####################################################################

USER_C_SRC   =
USER_CXX_SRC = $(USERINCLUDEPATHS)/Controller.cpp
USER_ASM_SRC =


####################################################################
# Additional compiler flags for C and C++                          #
####################################################################

CPPFLAGS += \
	-DDEBUG   \

CXXFLAGS +=          \
	-std=c++98         \
	-fno-exceptions    \
	-ftrapv            \
	-Wall              \
	-Wextra            \
	-Wfloat-equal      \
	-Wshadow           \
	-Wswitch-default   \
	-Wunreachable-code \
	-pedantic          \

CFLAGS +=         \
	-std=gnu99      \
	-fdata-sections \


####################################################################
# You might need to do changes to match your system setup          #
####################################################################

# Sourcery CodeBench tools from
# https://sourcery.mentor.com/GNUToolchain/release2322
LINUXCS   =
WINDOWSCS =
SYSTEMDIR = system
CMSISDIR  = $(SYSTEMDIR)/em_cmsis


ifdef SystemRoot
	TOOLDIR = $(ProgramFiles)/$(WINDOWSCS)
	RM = "$(TOOLDIR)/bin/cs-rm" -rf
else
	PATH_TEST_PRG = arm-none-eabi-gcc
	FULL_PRG      = $(shell which $(PATH_TEST_PRG))
	TOOLDIR       = $(patsubst %/bin/$(PATH_TEST_PRG),%,$(FULL_PRG))
	PATH_FLASH    = eACommander.sh
	FULL_FLASH    = $(shell which $(PATH_FLASH))
	RM            = rm -rf
endif

OBJ_DIR = build
EXE_DIR = bin
LST_DIR = lst

MAINFILE = main

CC      = "$(TOOLDIR)/bin/arm-none-eabi-gcc"
CXX_CC  = "$(TOOLDIR)/bin/arm-none-eabi-g++"
LD      = "$(TOOLDIR)/bin/arm-none-eabi-ld"
AR      = "$(TOOLDIR)/bin/arm-none-eabi-ar"
OBJCOPY = "$(TOOLDIR)/bin/arm-none-eabi-objcopy"
DUMP    = "$(TOOLDIR)/bin/arm-none-eabi-objdump"
SIZE    = "$(TOOLDIR)/bin/arm-none-eabi-size"

GCCVERSION = $(shell $(CC) -dumpversion)

.SUFFIXES:


####################################################################
# Flags                                                            #
####################################################################

LIBS +=         \
	-lc           \
	-lcs3         \
	-lcs3unhosted \

# -MMD : Don't generate dependencies on system header files.
# -MP  : Add phony targets, useful when a h-file is removed from a project.
# -MF  : Specify a file to write the dependencies to.

CPPFLAGS +=                                \
	-D$(DEVICE)                              \
	-MMD                                     \
	-MP                                      \
	-MF                                      \
	$(@:.o=.d)                               \
	-mcpu=$(CPU)                             \
	-Wa,-ahlms=$(LST_DIR)/$(@F:.o=.lst)      \
	-Os                                      \
	-mthumb                                  \
	-ffunction-sections                      \
	-DUSERCODE_FILENAME=\"$(PROJECTNAME).h\" \
	-DUSERCODE_CLASSNAME=$(PROJECTNAME)      \

ASFLAGS  += -Ttext 0x0

LDFLAGS +=                                                                      \
	-Xlinker                                                                      \
	-Map=$(LST_DIR)/main.map                                                      \
	-mcpu=$(CPU)                                                                  \
	-mthumb                                                                       \
	-L"$(TOOLDIR)/lib/gcc/arm-none-eabi/$(GCCVERSION)/thumb2"                     \
	-T$(CMSISDIR)/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/startup/cs3/efm32g.ld \
	-Wl,--gc-sections                                                             \
	-feliminate-unused-debug-symbols                                              \


####################################################################
# Source files                                                     #
####################################################################

SYSTEM_C_SRC +=                                                        \
	$(CMSISDIR)/CMSIS/CM3/CoreSupport/efm32lib/src/efm32_assert.c        \
	$(CMSISDIR)/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/system_efm32.c \
	$(CMSISDIR)/efm32lib/src/efm32_system.c                              \
	$(CMSISDIR)/efm32lib/src/efm32_usart.c                               \
	$(CMSISDIR)/efm32lib/src/efm32_cmu.c                                 \
	$(CMSISDIR)/efm32lib/src/efm32_emu.c                                 \
	$(CMSISDIR)/efm32lib/src/efm32_gpio.c                                \
	$(CMSISDIR)/efm32lib/src/efm32_adc.c                                 \
	$(CMSISDIR)/efm32lib/src/efm32_i2c.c                                 \

SYSTEM_CXX_SRC +=                                                 \
	$(SYSTEMDIR)/SentioEM3_HAL/time.cpp                             \
	$(SYSTEMDIR)/SentioEM3_HAL/RTC_DS3234.cpp                       \
	$(SYSTEMDIR)/SentioEM3_HAL/DebugInterface.cpp                   \
	$(SYSTEMDIR)/SentioEM3_HAL/System.cpp                           \
	$(SYSTEMDIR)/SentioEM3_HAL/AnalogInput.cpp                      \
	$(SYSTEMDIR)/SentioEM3_HAL/SensorExtensions/SHT1X_Sensirion.cpp \
	$(SYSTEMDIR)/SentioEM3_HAL/SensorExtensions/LTC2990.cpp         \
	$(SYSTEMDIR)/SentioEM3_HAL/SensorExtensions/ConfEH.cpp          \
	$(SYSTEMDIR)/SystemKernel/Statemachine.cpp                      \
	$(SYSTEMDIR)/SystemKernel/DriverInterface.cpp                   \
	$(SYSTEMDIR)/SystemKernel/main.cpp                              \

SYSTEM_ASM += \
	$(CMSISDIR)/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/startup/cs3/startup_efm32.s

SYSTEMINCLUDEPATHS +=                                   \
	$(CMSISDIR)/CMSIS/CM3/CoreSupport                     \
	$(CMSISDIR)/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32 \
	$(CMSISDIR)/efm32lib/inc                              \
	$(SYSTEMDIR)/efm32lib/src                             \
	$(SYSTEMDIR)/SentioEM3_HAL                            \
	$(SYSTEMDIR)/SentioEM3_HAL/SensorExtensions           \
	$(SYSTEMDIR)/SystemKernel                             \

INCLUDES +=                                                   \
	$(foreach includedir,$(SYSTEMINCLUDEPATHS),-I$(includedir)) \
	$(foreach includedir,$(USERINCLUDEPATHS),-I$(includedir))   \
	-I.                                                         \

C_SRC   = $(SYSTEM_C_SRC)   $(USER_C_SRC)
CXX_SRC = $(SYSTEM_CXX_SRC) $(USER_CXX_SRC)
S_SRC   = $(SYSTEM_ASM)     $(USER_ASM_SRC)


####################################################################
# Rules                                                            #
####################################################################

C_FILES   = $(notdir $(C_SRC))
CXX_FILES = $(notdir $(CXX_SRC))
S_FILES   = $(notdir $(S_SRC))

C_PATHS   = $(sort $(dir $(C_SRC) ))
CXX_PATHS = $(sort $(dir $(CXX_SRC)))
S_PATHS   = $(sort $(dir $(S_SRC)))

C_OBJS   = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.o))
CXX_OBJS = $(addprefix $(OBJ_DIR)/, $(CXX_FILES:.cpp=.o))
S_OBJS   = $(addprefix $(OBJ_DIR)/, $(S_FILES:.s=.o))

vpath %.c   $(C_PATHS)
vpath %.cpp $(CXX_PATHS)
vpath %.s   $(S_PATHS)

.PHONY: doc
doc:
	doxygen doc/Doxyfile

.PHONY: all
all: $(EXE_DIR)/$(MAINFILE).out $(EXE_DIR)/$(MAINFILE).bin flash

$(OBJ_DIR):
	mkdir $(OBJ_DIR)
	@- echo "Created build directory."

$(EXE_DIR):
	mkdir $(EXE_DIR)
	@- echo "Created executable directory."

$(LST_DIR):
	mkdir $(LST_DIR)
	@- echo "Created list directory."

$(OBJ_DIR)/%.o: %.cpp
	@- echo "Building file_CXX: $<"
	$(CXX_CC) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<
	$(SIZE) $@

$(OBJ_DIR)/%.o: %.c
	@- echo "Building file: $<"
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c -o $@ $<
	$(SIZE) $@
	
$(OBJ_DIR)/%.o: %.s
	@- echo "Assembling $<"
	$(CC) $(ASFLAGS) $(INCLUDES) -c -o $@ $<
	$(SIZE) $@

$(EXE_DIR)/$(MAINFILE).out: $(OBJ_DIR) $(EXE_DIR) $(LST_DIR) $(C_OBJS) $(S_OBJS) $(CXX_OBJS)
	@- echo "Linking target: $@"
	$(CXX_CC) $(LDFLAGS) $(C_OBJS) $(S_OBJS) $(CXX_OBJS) $(LIBS) -o $(EXE_DIR)/$(MAINFILE).out

$(EXE_DIR)/$(MAINFILE).bin: $(EXE_DIR)/$(MAINFILE).out
	@- echo "Creating binary file"
	$(OBJCOPY) -O binary $(EXE_DIR)/$(MAINFILE).out $(EXE_DIR)/$(MAINFILE).bin
	$(DUMP) -S $(EXE_DIR)/$(MAINFILE).out > $(EXE_DIR)/$(MAINFILE).list
	$(SIZE) --target=elf32-littlearm $(OBJ_DIR)/$(MAINFILE).o
	$(SIZE) --target=elf32-littlearm $(EXE_DIR)/$(MAINFILE).out

.PHONY: clean
clean:
	$(RM) $(OBJ_DIR) $(LST_DIR) $(EXE_DIR) doc/html doc/latex
	
.PHONY: flash
flash:
	sudo $(FULL_FLASH)                   \
		--verify                           \
		--mode out                         \
		--flash $(EXE_DIR)/$(MAINFILE).bin \
		--reset || true                    \
