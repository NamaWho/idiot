CONTIKI_PROJECT = alarm

all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1

# Include CoAP resources
MODULES_REL += ./resources

MODULES_REL += /home/iot_ubuntu_intel/.local/lib/python3.10/site-packages/emlearn


# Aggiungi json-c alla lista delle librerie
TARGET_LIBFILES += -lm -ljson-c

INC += /home/iot_ubuntu_intel/.local/lib/python3.10/site-packages/emlearn

CONTIKI=../../..

# Include the CoAP implementation
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

include $(CONTIKI)/Makefile.include
