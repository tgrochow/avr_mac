TARGET=avr_template
MCU=atmega2560
FCPU=16000000
PROGRAMMER=wiring
PORT=/dev/ttyACM0
PBAUD=115200
SBAUD=9600

BINDIR=bin
OBJDIR=build
SRCDIR=src
INCDIR=include
INCLUDES=-I$(INCDIR) -I../uart/$(INCDIR)
SOURCES=$(notdir $(wildcard $(SRCDIR)/*.c))
OBJECTS=$(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
LIBS=-L../uart/lib -luart

CFLAGS=$(INCLUDES) -c -Os -DF_CPU='$(FCPU)' -DBAUD='$(SBAUD)UL'
LDFLAGS=$(LIBS)

all: project_structure hex

hex: $(BINDIR)/$(TARGET).hex

$(BINDIR)/$(TARGET).hex: $(BINDIR)/$(TARGET).elf
	avr-objcopy -O ihex -j .data -j .text $(BINDIR)/$(TARGET).elf                \
	$(BINDIR)/$(TARGET).hex

$(BINDIR)/$(TARGET).elf: $(OBJECTS)
	avr-gcc -mmcu=$(MCU) $(OBJECTS) $(LDFLAGS) -o $(BINDIR)/$(TARGET).elf

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	avr-gcc $(CFLAGS) -mmcu=$(MCU) $< -o $@

program:
	avrdude -p$(MCU) -P$(PORT) -B$(PBAUD) -c$(PROGRAMMER)                        \
	-Uflash:w:$(BINDIR)/$(TARGET).hex:a

project_structure:
	mkdir -p $(OBJDIR) $(BINDIR)

size:
	avr-size --mcu=$(MCU) -C $(BINDIR)/$(TARGET).elf

clean: clean_tmp clean_bin

clean_tmp:
	rm -r $(OBJDIR)

clean_bin:
	rm -rf $(BINDIR)

serial_monitor:
	stty -F $(PORT) cs8 $(SBAUD) ignbrk -brkint -icrnl -imaxbel -opost -onlcr    \
	-isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon      \
	-crtscts min 0
	clear
	tail -f $(PORT)