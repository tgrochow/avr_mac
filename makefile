TARGET=avr_template
MCU=atmega2560
PROGRAMMER=wiring
PORT=-P/dev/ttyACM0
BAUD=-B115200

BINDIR=./bin
OBJDIR=./build
SRCDIR=./src
SOURCES=main.c
OBJECTS=$(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
CFLAGS=-c -Os
LDFLAGS=

all: project_structure hex

hex: $(BINDIR)/$(TARGET).hex

$(BINDIR)/$(TARGET).hex: $(BINDIR)/$(TARGET).elf
	avr-objcopy -O ihex -j .data -j .text $(BINDIR)/$(TARGET).elf $(BINDIR)/$(TARGET).hex

$(BINDIR)/$(TARGET).elf: $(OBJECTS)
	avr-gcc $(LDFLAGS) -mmcu=$(MCU) $(OBJECTS) -o $(BINDIR)/$(TARGET).elf

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	avr-gcc $(CFLAGS) -mmcu=$(MCU) $< -o $@

program: hex
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -Uflash:w:$(BINDIR)/$(TARGET).hex:a

clean: clean_tmp clean_bin

clean_tmp:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(BINDIR)/*.elf

clean_bin:
	rm -rf $(BINDIR)/*.hex

project_structure:
	mkdir -p $(OBJDIR) $(BINDIR)