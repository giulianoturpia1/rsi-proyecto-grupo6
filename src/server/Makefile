CONTIKI_PROJECT = udp-server
all: $(CONTIKI_PROJECT)

PLATFORMS_EXCLUDE = sky z1
CONTIKI = ../../../../contiki-ng

# Orchestra?
MAKE_WITH_ORCHESTRA ?= 0
# RPL storing mode?
MAKE_WITH_STORING_ROUTING ?= 0

# Modulos Fijos
MODULES += $(CONTIKI_NG_SERVICES_DIR)/shell
MODULES_REL += $(TARGET)
MAKE_MAC = MAKE_MAC_TSCH


# Makefiles de Contiki-NG
include $(CONTIKI)/Makefile.dir-variables
include $(CONTIKI)/Makefile.identify-target

ifneq ($(TARGET),cooja)
	ORCHESTRA_EXTRA_RULES = &unicast_per_neighbor_rpl_ns
	# Habilitar el modo Orchestra
	ifeq ($(MAKE_WITH_ORCHESTRA),1)
		MODULES += $(CONTIKI_NG_SERVICES_DIR)/orchestra

		ifeq ($(MAKE_WITH_STORING_ROUTING),1)
			# Habilitar `rpl_storing` rule
			ORCHESTRA_EXTRA_RULES = &unicast_per_neighbor_rpl_storing
		endif
	endif

	# Pasar las reglas de Orchestra al compilador
	CFLAGS += -DORCHESTRA_CONF_RULES="{&eb_per_time_source,$(ORCHESTRA_EXTRA_RULES),&default_common}"

	ifeq ($(MAKE_WITH_STORING_ROUTING),1)
		MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC
		CFLAGS += -DRPL_CONF_MOP=RPL_MOP_STORING_NO_MULTICAST
	endif
endif

ifeq ($(TARGET),cooja)  # Comentar o descomentar para habilitar/deshabilitar los modos adicionales
	MODULES += $(CONTIKI_NG_SERVICES_DIR)/orchestra
	# Habilitar el modo Orchestra con `rpl_storing` rule
	MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC
endif

include $(CONTIKI)/Makefile.include
