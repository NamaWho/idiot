#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "sys/etimer.h"
#include "os/dev/leds.h"
#include "coap-blocking-api.h"

#include "voltage_status.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define SERVER_EP "coap://[fd00::1]:5683"

#define MAX_REGISTRATION_RETRY 3


static int max_registration_retry = MAX_REGISTRATION_RETRY;

void client_chunk_handler(coap_message_t *response)
{
	if(response == NULL) {

		LOG_ERR("Request timed out\n");

	}else if(response->code != 65){

		LOG_ERR("Error: %d\n",response->code);

	}else{

		LOG_INFO("Registration successful\n");
		max_registration_retry = 0;		// if = 0 --> registration ok!

		return;
	}
	
	// If I'm at this point, there was some problem in the registration phasse, so we decide to try again until max_registration_retry != 0
	max_registration_retry--;
	if(max_registration_retry==0)
		max_registration_retry=-1;
}

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t res_voltage;
extern coap_resource_t res_voltage_status;

static struct etimer e_timer, sleep_timer;

PROCESS(voltage_server, "Voltage Sensor CoAP Server");
AUTOSTART_PROCESSES(&voltage_server);

int status = 1;

PROCESS_THREAD(voltage_server, ev, data)
{
  static coap_endpoint_t main_server_ep;
  static coap_message_t request[1];

  PROCESS_BEGIN();
  
  LOG_INFO("Starting Voltage Server\n");

  while(max_registration_retry!=0){
		/* -------------- REGISTRATION --------------*/
		// Populate the coap_endpoint_t data structure
		coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &main_server_ep);
		// Prepare the message
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
    coap_set_header_uri_path(request, "register/");
		const char msg[] = "voltage";
		//Set payload
		coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

    leds_single_on(LEDS_YELLOW);
	
		 COAP_BLOCKING_REQUEST(&main_server_ep, request, client_chunk_handler);
    
		/* -------------- END REGISTRATION --------------*/
		if(max_registration_retry == -1){		// something goes wrong more MAX_REGISTRATION_RETRY times, node goes to sleep then try again
			etimer_set(&sleep_timer, 30*CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleep_timer));
			max_registration_retry = MAX_REGISTRATION_RETRY;
		}
	}


  LOG_INFO("REGISTRATION SUCCESS\n")
  leds_single_off(LEDS_YELLOW);
  

  coap_activate_resource(&res_voltage, "voltage");
  coap_activate_resource(&res_voltage_status, "voltage/status");


  if(max_registration_retry == 0 && status == 1){
    // set a timer to send the voltage value every 10 seconds
    etimer_set(&e_timer, CLOCK_SECOND * 10);
    
    while (1) {
      PROCESS_WAIT_EVENT();

      if (ev == PROCESS_EVENT_TIMER && data == &e_timer){
        if (status == 1){
          res_pressure.trigger();
          LOG_INFO("Voltage event triggered\n");
          etimer_reset(&e_timer);
        }
        else{
          LOG_ERR("Voltage sensor is off\n");
          break;
        }
      }
    }
  }

  PROCESS_END();
}
