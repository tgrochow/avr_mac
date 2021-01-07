# config________________________________________________________________________

include ../config

TARGET=crypto_util
BINDIR=bin
LIBDIR=lib
OBJDIR=build
SRCDIR=src
INCDIR=include
INCLUDES=-I$(INCDIR) -I../uart/$(INCDIR)
SOURCES=$(notdir $(wildcard $(SRCDIR)/*.c))
OBJECTS=$(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
LIB_OBJECTS=$(filter-out $(OBJDIR)/main.o,$(OBJECTS))
LIBS=-L../uart/lib -luart

CFLAGS=$(INCLUDES) -c -Os -DF_CPU='$(FCPU)' -DBAUD='$(SBAUD)UL'
LDFLAGS=$(LIBS)

#_______________________________________________________________________________

# targets_______________________________________________________________________

all: project_structure hex lib

hex: $(BINDIR)/$(TARGET).hex

lib: $(LIBDIR)/lib$(TARGET).a

program:
	avrdude -p$(MCU) -P$(PORT) -B$(PBAUD) -c$(PROGRAMMER)                        \
	-Uflash:w:$(BINDIR)/$(TARGET).hex:a

project_structure:
	mkdir -p $(OBJDIR) $(BINDIR) $(LIBDIR)

size:
	avr-size --mcu=$(MCU) -C $(BINDIR)/$(TARGET).elf

clean: clean_tmp clean_bin clean_lib

clean_tmp:
	rm -rf $(OBJDIR)

clean_bin:
	rm -rf $(BINDIR)

clean_lib:
	rm -rf $(LIBDIR)

serial_monitor:
	stty -F $(PORT) cs8 $(SBAUD) ignbrk -brkint -icrnl -imaxbel -opost -onlcr    \
	-isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon      \
	-crtscts min 0
	clear
	tail -f $(PORT)

#_______________________________________________________________________________

# rules_________________________________________________________________________

$(BINDIR)/$(TARGET).hex: $(BINDIR)/$(TARGET).elf
	avr-objcopy -O ihex -j .data -j .text $(BINDIR)/$(TARGET).elf                \
	$(BINDIR)/$(TARGET).hex

$(BINDIR)/$(TARGET).elf: $(OBJECTS)
	avr-gcc -mmcu=$(MCU) $(OBJECTS) $(LDFLAGS) -o $(BINDIR)/$(TARGET).elf

$(LIBDIR)/lib$(TARGET).a: $(LIB_OBJECTS)
	avr-ar rcs $@ $(LIB_OBJECTS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	avr-gcc $(CFLAGS) -mmcu=$(MCU) $< -o $@

#_______________________________________________________________________________
