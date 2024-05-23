CONTIKI_PROJECT = alarm

all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1

# Include CoAP resources
MODULES_REL += ./resources
MODULES_REL += ./utils
MODULES_REL += ./ml
MODULES_REL += /home/iot_ubuntu_intel/.local/lib/python3.10/site-packages/emlearn
MODULES_REL += /Users/danielnamaki/.pyenv/versions/3.12.2/lib/python3.12/site-packages/emlearn

TARGET_LIBFILES += -lm

INC += /home/iot_ubuntu_intel/.local/lib/python3.10/site-packages/emlearn
INC += /Users/danielnamaki/.pyenv/versions/3.12.2/lib/python3.12/site-packages/emlearn

CONTIKI=../../..

# Include the CoAP implementation
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

include $(CONTIKI)/Makefile.include
