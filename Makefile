####################################################################
# Directories to be included                                       #
####################################################################

# PROJECTNAME      = Algorithms
PROJECTNAME      = Controller

USERINCLUDEPATHS = src
SYSTEMDIR        = system


####################################################################
# User source files                                                #
####################################################################

USER_C_SRC   =
USER_CXX_SRC = $(USERINCLUDEPATHS)/$(PROJECTNAME).cpp
USER_ASM_SRC =


####################################################################
# Additional compiler flags for C and C++                          #
####################################################################

CPPFLAGS += \
	-DDEBUG   \

CXXFLAGS += \
	-std=c++98               \
	-fno-exceptions          \
	-ftrapv                  \
	-Wall                    \
	-Wextra                  \
	-Wfloat-equal            \
	-Wshadow                 \
	-Wswitch-default         \
	-Wunreachable-code       \
	-Wno-int-to-pointer-cast \
	-pedantic                \

CFLAGS += \
	-std=gnu99      \
	-fdata-sections \


####################################################################
# Include system make file                                         #
####################################################################

include $(SYSTEMDIR)/Makefile
