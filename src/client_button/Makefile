CONTIKI_PROJECT = udp-client	
all: $(CONTIKI_PROJECT)

# Necessary Modules
MODULES_REL += $(TARGET)
MAKE_MAC = MAKE_MAC_TSCH
MODULES += os/services/shell

CONTIKI = ../../../contiki-ng

include $(CONTIKI)/Makefile.include
