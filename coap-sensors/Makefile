CONTIKI_PROJECT = pressure rotation vibration voltage

all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1

# Include CoAP resources
MODULES_REL += ./resources

CONTIKI=../../..

# Include the CoAP implementation
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

# Add the utils directory
MODULES_REL += ./resources/utils

LDLIBS += -lm

include $(CONTIKI)/Makefile.include
