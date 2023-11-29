CONTIKI_PROJECT = udp-client
all: $(CONTIKI_PROJECT)

# Target and Board
#TARGET = cc26x0-cc13x0
#BOARD = launchpad/cc1350

#Necessary Modules
MODULES_REL += $(TARGET)
MAKE_MAC = MAKE_MAC_TSCH
MODULES += os/services/shell

CONTIKI = ../../../contiki-ng

include $(CONTIKI)/Makefile.include
