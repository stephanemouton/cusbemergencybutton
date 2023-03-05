#
# cusbemergencybutton Makefile
# Manual version
#

# Compiler settings
CXX     = g++
CFLAGS  = -O2 -fPIC -Wall -ansi -pedantic

# Linker settings
EXE     = usbemergencybutton
LIBS    = -liniparser
LDFLAGS = -Wall 
BUILD   = build/$(EXE)

# other settings
INSTDIR = /usr/bin
RM      = rm -f

# Implicit rules
COMPILE.c=$(CXX) $(CFLAGS) -c
.c.o:
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)

SRCS =  src/eventmanager.c \
		src/parameters.c \
		src/usbemergency.c

OBJS = $(SRCS:.c=.o)

# Main target
all: $(EXE)

$(EXE) : $(OBJS) 
	$(CXX) $(LDFLAGS) $(LIBS) -o $(BUILD) $(OBJS)

install: $(BUILD)
	mv $(BUILD) ${INSTDIR}
 
clean:
	$(RM) $(OBJS) $(BUILD)
