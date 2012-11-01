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

USER_FLAGS =         \
	-Wall              \
	-Wextra            \
	-pedantic          \
	-ftrapv            \
	-Wfloat-equal      \
	-Wshadow           \
	-Wswitch-default   \
	-Wunreachable-code \
	-DDEBUG            \


####################################################################
# You might need to do changes to match your system setup          #
####################################################################

# CodeSourcery tools from (registration required)
# http://www.codesourcery.com/sgpp/lite/arm/portal/release1802
LINUXCS   =
WINDOWSCS =
SYSTEMDIR = system

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
DUMP    = "$(TOOLDIR)/bin/arm-none-eabi-objdump" --disassemble

GCCVERSION = $(shell $(CC) -dumpversion)

.SUFFIXES:


####################################################################
# Flags                                                            #
####################################################################

# -MMD : Don't generate dependencies on system header files.
# -MP  : Add phony targets, useful when a h-file is removed from a project.
# -MF  : Specify a file to write the dependencies to.
DEPFLAGS =   \
	-MMD       \
	-MP        \
	-MF        \
	$(@:.o=.d) \

LIBS +=         \
	-lc           \
	-lcs3         \
	-lcs3unhosted \

COMMONFLAGS +=                             \
	$(USER_FLAGS)                            \
	-D$(DEVICE)                              \
	$(DEPFLAGS)                              \
	-mcpu=$(CPU)                             \
	-Wa,-ahlms=$(LST_DIR)/$(@F:.o=.lst)      \
	-Os                                      \
	-mthumb                                  \
	-ffunction-sections                      \
	-DUSERCODE_FILENAME=\"$(PROJECTNAME).h\" \
	-DUSERCODE_CLASSNAME=$(PROJECTNAME)      \

CFLAGS =          \
	$(COMMONFLAGS)  \
	-fdata-sections \
	-std=c99        \

CXXFLAGS +=       \
	$(COMMONFLAGS)  \
$(USERCFLAGS)     \
	-fno-exceptions \
	-std=c++98      \

ASFLAGS  += -Ttext 0x0

LDFLAGS +=                                                                       \
	-Xlinker                                                                       \
	-Map=$(LST_DIR)/main.map                                                       \
	-mcpu=$(CPU)                                                                   \
	-mthumb                                                                        \
	-T$(SYSTEMDIR)/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/startup/cs3/efm32g.ld \
	-L"$(TOOLDIR)/bin/arm-none-eabi/lib/thumb2"                                    \
	-L"$(TOOLDIR)/lib/gcc/arm-none-eabi/$(GCCVERSION)/thumb2"                      \
	-Wl,--gc-sections                                                              \
	-feliminate-unused-debug-symbols                                               \


####################################################################
# Source files                                                     #
####################################################################

SYSTEM_C_SRC +=                                                         \
	$(SYSTEMDIR)/CMSIS/CM3/CoreSupport/efm32lib/src/efm32_assert.c        \
	$(SYSTEMDIR)/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/system_efm32.c \
	$(SYSTEMDIR)/efm32lib/src/efm32_system.c                              \
	$(SYSTEMDIR)/efm32lib/src/efm32_usart.c                               \
	$(SYSTEMDIR)/efm32lib/src/efm32_cmu.c                                 \
	$(SYSTEMDIR)/efm32lib/src/efm32_emu.c                                 \
	$(SYSTEMDIR)/efm32lib/src/efm32_gpio.c                                \

SYSTEM_CXX_SRC +=                                                 \
	$(SYSTEMDIR)/SentioEM3_HAL/time.cpp                             \
	$(SYSTEMDIR)/SentioEM3_HAL/RTC_DS3234.cpp                       \
	$(SYSTEMDIR)/SentioEM3_HAL/DebugInterface.cpp                   \
	$(SYSTEMDIR)/SentioEM3_HAL/System.cpp                           \
	$(SYSTEMDIR)/SentioEM3_HAL/SensorExtensions/SHT1X_Sensirion.cpp \
	$(SYSTEMDIR)/SentioEM3_HAL/SensorExtensions/LTC2990.cpp         \
	$(SYSTEMDIR)/SystemKernel/Statemachine.cpp                      \
	$(SYSTEMDIR)/SystemKernel/DriverInterface.cpp                   \
	$(SYSTEMDIR)/SystemKernel/main.cpp                              \

SYSTEM_ASM += \
	$(SYSTEMDIR)/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/startup/cs3/startup_efm32.s

SYSTEMINCLUDEPATHS +=                                    \
	$(SYSTEMDIR)/CMSIS/CM3/CoreSupport                     \
	$(SYSTEMDIR)/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32 \
	$(SYSTEMDIR)/efm32lib/inc                              \
	$(SYSTEMDIR)/SentioEM3_HAL                             \
	$(SYSTEMDIR)/SentioEM3_HAL/SensorExtensions            \
	$(SYSTEMDIR)/SystemKernel                              \
	$(SYSTEMDIR)/efm32lib/src                              \

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
	$(CXX_CC) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<
	$(TOOLDIR)/bin/arm-none-eabi-size  $@

$(OBJ_DIR)/%.o: %.c
	@- echo "Building file: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
	$(TOOLDIR)/bin/arm-none-eabi-size  $@
	
$(OBJ_DIR)/%.o: %.s
	@- echo "Assembling $<"
	$(CC) $(ASFLAGS) $(INCLUDES) -c -o $@ $<
	$(TOOLDIR)/bin/arm-none-eabi-size $@

$(EXE_DIR)/$(MAINFILE).out: $(OBJ_DIR) $(EXE_DIR) $(LST_DIR) $(C_OBJS) $(S_OBJS) $(CXX_OBJS)
	@- echo "Linking target: $@"
	$(CXX_CC) $(LDFLAGS) $(C_OBJS) $(S_OBJS) $(CXX_OBJS) $(LIBS) -o $(EXE_DIR)/$(MAINFILE).out

$(EXE_DIR)/$(MAINFILE).bin: $(EXE_DIR)/$(MAINFILE).out
	@- echo "Creating binary file"
	$(OBJCOPY) -O binary $(EXE_DIR)/$(MAINFILE).out $(EXE_DIR)/$(MAINFILE).bin
	$(TOOLDIR)/bin/arm-none-eabi-objdump -S $(EXE_DIR)/$(MAINFILE).out > $(EXE_DIR)/$(MAINFILE).list
	$(TOOLDIR)/bin/arm-none-eabi-size --target=elf32-littlearm $(OBJ_DIR)/$(MAINFILE).o
	$(TOOLDIR)/bin/arm-none-eabi-size --target=elf32-littlearm $(EXE_DIR)/$(MAINFILE).out

.PHONY: clean
clean:
	$(RM) $(OBJ_DIR) $(LST_DIR) $(EXE_DIR)
	
.PHONY: flash
flash:
	sudo $(FULL_FLASH)                   \
		--verify                           \
		--mode out                         \
		--flash $(EXE_DIR)/$(MAINFILE).bin \
		--reset || true                    \
