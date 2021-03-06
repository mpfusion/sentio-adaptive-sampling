####################################################################
# Directories to be included                                       #
####################################################################

# PROJECTNAME      = Controller
PROJECTNAME      = Algorithms
ALGORITHM        = 2

USERINCLUDEPATHS = src
SYSTEMDIR        = system


####################################################################
# User source files                                                #
####################################################################

USER_C_SRC   =

USER_CXX_SRC = \
	$(USERINCLUDEPATHS)/$(PROJECTNAME).cpp \
	$(USERINCLUDEPATHS)/Configuration.cpp  \
	$(USERINCLUDEPATHS)/EWMA.cpp           \
	$(USERINCLUDEPATHS)/WCMA.cpp           \

USER_ASM_SRC =


####################################################################
# Additional compiler flags for C and C++                          #
####################################################################

CPPFLAGS += \
	-DDEBUG                  \
	-DALGORITHM=$(ALGORITHM) \

	# -D$(ALGORITHM) \

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
