###################################################################################################

F_CPU=16000000UL
DEVICE=attiny45
DEV=t45
PROGRAMMER=avrisp2
DEBUG=

###################################################################################################

PORT=usb

###################################################################################################

CC=avr-gcc
CXX=avr-g++
OBJCOPY=avr-objcopy

###################################################################################################

IFLAGS=
CFLAGS=-g -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(DEVICE) -DDEBUG=$(DEBUG) $(IFLAGS)
CXXFLAGS=-g -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(DEVICE) -DDEBUG=$(DEBUG) $(IFLAGS)

###################################################################################################

BIN=$(BUILDDIR)/doorsensor
PWD=$(`pwd`)
SOURCEDIR=$(PWD)src
HEADERDIR=$(PWD)inc
BUILDDIR=$(PWD)bin
CSOURCES := $(shell find $(SOURCEDIR) -name '*.c')
CXXSOURCES := $(shell find $(SOURCEDIR) -name '*.cpp')
SSOURCES :=
OBJECTS := $(addprefix $(BUILDDIR)/,$(CSOURCES:%.c=%.c.o)) 		\
	$(addprefix $(BUILDDIR)/,$(CXXSOURCES:%.cpp=%.cpp.o))

###################################################################################################

dir_guard=@mkdir -p $(@D)       

${BIN}.hex: ${BIN}.elf
	${OBJCOPY} -O ihex -R .eeprom $< $@

${BIN}.elf: $(OBJECTS)
	${CXX} ${IFLAGS} -I$(HEADERDIR) -I$(dir $<) -mmcu=$(DEVICE) -o $@ $^

$(BUILDDIR)/%.c.o: %.c
	$(dir_guard)
	$(CC) ${CFLAGS} -I$(HEADERDIR) -I$(dir $<) -c $< -o $@

$(BUILDDIR)/%.cpp.o: %.cpp
	$(dir_guard)
	$(CXX) ${CXXFLAGS} -I$(HEADERDIR) -I$(dir $<) -c $< -o $@

echo:
	@echo $(CSOURCES)
	@echo $(CXXSOURCES)
	@echo $(OBJECTS)


install: ${BIN}.hex
	avrdude -v -e -p$(DEV) -c$(PROGRAMMER) -P$(PORT) -Uflash:w:$<

clean:
	rm -f ${BIN}.elf ${BIN}.hex ${OBJECTS}
