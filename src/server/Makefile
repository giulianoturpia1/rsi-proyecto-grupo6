CONTIKI_PROJECT = udp-server
all: $(CONTIKI_PROJECT)

#TARGET = cc26x0-cc13x0
#BOARD = launchpad/cc1350

CONTIKI=/home/user/contiki-ng
# Enable MAC TSCH
MAKE_MAC = MAKE_MAC_TSCH 

# Force Orchestra from command line
MAKE_WITH_ORCHESTRA ?= 0
# RPL storing mode?
MAKE_WITH_STORING_ROUTING ?= 0
# Orchestra link-based rule? (Works only if Orchestra & storing mode routing is enabled)
MAKE_WITH_LINK_BASED_ORCHESTRA ?= 0


# Import useful Contiki-NG configuration variables
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_SERVICES_DIR)/shell

ORCHESTRA_EXTRA_RULES = &unicast_per_neighbor_rpl_ns

# Enable the Orchestra module if requested, dependant on the routing mode, storing or non-storing
ifeq ($(MAKE_WITH_ORCHESTRA),1)
  MODULES += $(CONTIKI_NG_SERVICES_DIR)/orchestra

  ifeq ($(MAKE_WITH_STORING_ROUTING),1)
    ifeq ($(MAKE_WITH_LINK_BASED_ORCHESTRA),1)
      # Enable the `link_based` rule
      ORCHESTRA_EXTRA_RULES = &unicast_per_neighbor_link_based
    else
      # Enable the `rpl_storing` rule
      ORCHESTRA_EXTRA_RULES = &unicast_per_neighbor_rpl_storing
    endif

  else
    ifeq ($(MAKE_WITH_LINK_BASED_ORCHESTRA),1)
      $(error "Inconsistent configuration: link-based Orchestra requires routing info")
    endif

  endif

  # Pass the Orchestra rules to the compiler
  CFLAGS += -DORCHESTRA_CONF_RULES="{&eb_per_time_source,$(ORCHESTRA_EXTRA_RULES),&default_common}"
endif

# Enable the storing mode if requested
ifeq ($(MAKE_WITH_STORING_ROUTING),1)
  MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC
  CFLAGS += -DRPL_CONF_MOP=RPL_MOP_STORING_NO_MULTICAST
endif

# Import the Contiki-NG configuration
include $(CONTIKI)/Makefile.include
