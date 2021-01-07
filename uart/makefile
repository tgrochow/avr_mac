include ../config

TARGET=uart
LIBDIR=lib
OBJDIR=build
SRCDIR=src
INCDIR=include
SOURCES=$(notdir $(wildcard $(SRCDIR)/*.c))
OBJECTS=$(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
CFLAGS=-I $(INCDIR) -c -Os -DF_CPU='$(FCPU)' -DBAUD='$(SBAUD)UL'
LDFLAGS=

all: project_structure lib

lib: $(LIBDIR)/lib$(TARGET).a

$(LIBDIR)/lib$(TARGET).a: $(OBJECTS)
	avr-ar rcs $@ $(OBJECTS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	avr-gcc $(CFLAGS) -mmcu=$(MCU) $< -o $@

project_structure:
	mkdir -p $(OBJDIR) $(LIBDIR)

clean: clean_tmp clean_lib

clean_tmp:
	rm -rf $(OBJDIR)

clean_lib:
	rm -rf $(LIBDIR)
