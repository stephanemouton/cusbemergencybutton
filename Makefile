#
# cusbemergencybutton Makefile
# Manual version
#

# Compiler settings
CXX     = g++
CFLAGS  = -O2 -fPIC -Wall -ansi -pedantic

# Linker settings
EXE     	= usbemergencybutton
TOOL		= get_key_from_device
LIBS_EXE    = -liniparser
LDFLAGS		= -Wall 
BUILD_EXE	= build/$(EXE)
BUILD_TOOL	= tools/$(TOOL)

# other settings
INSTDIR		= /usr/local/bin

# Implicit rules
COMPILE.c=$(CXX) $(CFLAGS) -c
.c.o:
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)

SRCS_EXE =	src/eventmanager.c \
			src/parameters.c \
			src/pidfile.c \
			src/usbemergency.c
OBJS_EXE = $(SRCS_EXE:.c=.o)

SRCS_TOOL =	src/eventmanager.c \
			src/get_key_from_device.c
OBJS_TOOL = $(SRCS_TOOL:.c=.o)

# Main target
all: $(EXE) $(TOOL)

$(EXE) : $(OBJS_EXE) 
	$(CXX) $(LDFLAGS) $(LIBS_EXE) -o $(BUILD_EXE) $(OBJS_EXE)

$(TOOL) : $(OBJS_TOOL) 
	$(CXX) $(LDFLAGS) $(LIBS_TOOL) -o $(BUILD_TOOL) $(OBJS_TOOL)

install: $(BUILD_EXE)
	mv $(BUILD_EXE) ${INSTDIR}
 
clean:
	$(RM) $(OBJS_EXE) $(BUILD_EXE) $(OBJS_TOOL) $(BUILD_TOOL)
