#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "sys/etimer.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t res_voltage;

static struct etimer e_timer;

PROCESS(voltage_server, "Voltage Sensor CoAP Server");
AUTOSTART_PROCESSES(&voltage_server);

PROCESS_THREAD(voltage_server, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  LOG_INFO("Starting Server\n");

  coap_activate_resource(&res_voltage, "voltage");

  etimer_set(&e_timer, CLOCK_SECOND * 4);

  printf("Loop\n");

  while (1)
  {
    PROCESS_WAIT_EVENT();

    if (ev == PROCESS_EVENT_TIMER && data == &e_timer)
    {
      printf("Event triggered\n");

      res_voltage.trigger();

      etimer_set(&e_timer, CLOCK_SECOND * 4);
    }
  }

  PROCESS_END();
}
