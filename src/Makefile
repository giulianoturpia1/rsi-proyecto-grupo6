TARGET = cc26x0-cc13x0
BOARD = launchpad/cc1350

CONTIKI_PROJECT = netstack_event
all: $(CONTIKI_PROJECT)

MAKE_MAC = MAKE_MAC_TSCH

CONTIKI = ../../contiki-ng
MODULES += $(CONTIKI)/os/services/shell

include $(CONTIKI)/Makefile.include