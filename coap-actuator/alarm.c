#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
#include "DT_model.h"

// button library
#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

// led library
#include "dev/leds.h"


// include ml model to predict maintenance need

/* ------- Log configuration ------- */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
/* ------------------------------- */


/* ------ CoAP resources ------ */
#define SERVER_EP "coap://[fd00::1]:5683"

#define ROTATION_SERVER_EP "coap://[fd00::203:3:3:3]:5683" 
#define VOLTAGE_SERVER_EP "coap://[fd00::205:5:5:5]:5683" 
#define PRESSURE_SERVER_EP "coap://[fd00::202:2:2:2]:5683" 
#define VIBRATION_SERVER_EP "coap://[fd00::204:4:4:4]:5683" 


char *service_url = "/telemetry";

#define TOGGLE_INTERVAL 30

/*----------------------------------------------------------------------------*/
//static uip_ipaddr_t rotation_server_ipaddr[1]; /* holds the server ip address */
static coap_observee_t *obs_rotation;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//static uip_ipaddr_t voltage_server_ipaddr[1] ; /* holds the server ip address */
static coap_observee_t *obs_voltage;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//static uip_ipaddr_t pressure_server_ipaddr[1]; /* holds the server ip address */
static coap_observee_t *obs_pressure;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//static uip_ipaddr_t vibration_server_ipaddr[1]; /* holds the server ip address */
static coap_observee_t *obs_vibration;

/*----------------------------------------------------------------------------*/

PROCESS(alarm_client, "Alarm Actuator Client");
AUTOSTART_PROCESSES(&alarm_client);

/* ------ Machine Configuration ------ */

/*static uint8_t rotation = 0;
static uint8_t voltage = 0;
static uint8_t pressure = 0;
static uint8_t vibration = 0;

static uint8_t error_one_count = 0;
static uint8_t error_two_count = 0;
static uint8_t error_three_count = 0;
static uint8_t error_four_count = 0;
static uint8_t error_five_count = 0;*/

/* ----------------------------------- */

/*----------------------------------------------------------------------------*/
/*
 * Handle the response to the observe request and the following notifications
 */
static void
notification_callback(coap_observee_t *obs, void *notification,
                      coap_notification_flag_t flag)
{
  int len = 0;
  const uint8_t *payload = NULL;

  printf("Notification handler\n");
  printf("Observee URI: %s\n", obs->url);
  if(notification) {
    len = coap_get_payload(notification, &payload);

  }
  switch(flag) {
    
  case NOTIFICATION_OK:
    printf("NOTIFICATION OK: %*s\n", len, (char *)payload);
    break;

  case OBSERVE_OK: /* server accepeted observation request */
    printf("OBSERVE_OK: %*s\n", len, (char *)payload);
    break;
  case OBSERVE_NOT_SUPPORTED:
    printf("OBSERVE_NOT_SUPPORTED: %*s\n", len, (char *)payload);
    obs = NULL;
    break;
  case ERROR_RESPONSE_CODE:
    printf("ERROR_RESPONSE_CODE: %*s\n", len, (char *)payload);
    obs = NULL;
    break;
  case NO_REPLY_FROM_SERVER:
    printf("NO_REPLY_FROM_SERVER: "
           "removing observe registration with token %x%x\n",
           obs->token[0], obs->token[1]);
    obs = NULL;
    break;
  }
}
/*----------------------------------------------------------------------------*/
/*
 * Toggle the observation of the remote resource
 */
void
toggle_observation(coap_observee_t *obs, coap_endpoint_t *server_ep, char *res_uri)
{
  if(obs) {
    printf("Stopping observation\n");
    coap_obs_remove_observee(obs);
    obs = NULL;
  } else {
    printf("Starting observation\n");
    obs = coap_obs_request_registration(server_ep, res_uri, notification_callback, NULL);
  }
}

/*----------------------------------------------------------------------------*/
/*
void client_chunk_handler(coap_message_t *response)
{
    const uint8_t *chunk;

    if (response == NULL)
    {
        puts("Request timed out");
        return;
    }

    int len = coap_get_payload(response, &chunk);

    printf("|%.*s", len, (char *)chunk);

    // i receive {"sensor": "rotation", "payload": {"standard_deviation": 0.0, "mean": 0.0}}
    // i need to parse the json and get the values of standard_deviation and mean
    // then i need to compare the values with the ml model to predict maintenance need
    // if the values are greater than the threshold, i need to send a post request to the actuator to turn on the alarm

    json_object *parsed_json = json_tokener_parse((char *)chunk);
    json_object *sensor = json_object_object_get(parsed_json, "sensor");
    json_object *payload = json_object_object_get(parsed_json, "payload");

    json_object *standard_deviation = json_object_object_get(payload, "standard_deviation");
    json_object *mean = json_object_object_get(payload, "mean");


    switch (json_object_get_string(sensor))
    {
    case "rotation":
        
        break;
    case "voltage":
        
        break;
    case "pressure":
        
        break;
    case "vibration":  
       
        break;

    default:
        break;
    }
  
    
}*/

/*void client_chunk_handler_registration(coap_message_t *response)
{
	const uint8_t *chunk;

	if(response == NULL) {
		LOG_INFO("Request timed out");
		return;
	}
	registered = true;
	int len = coap_get_payload(response, &chunk);
	LOG_INFO("|%.*s \n", len, (char *)chunk);
}
*/

PROCESS_THREAD(alarm_client, ev, data)
{
   // static coap_endpoint_t main_server_ep;
    static coap_endpoint_t rotation_server_ep;
    static coap_endpoint_t voltage_server_ep;
    static coap_endpoint_t pressure_server_ep;
    static coap_endpoint_t vibration_server_ep;

    //set a timer to print the something
    static struct etimer et;
    
    //static coap_message_t request[4];

    PROCESS_BEGIN();

    // set the timer
    etimer_set(&et, 5 * CLOCK_SECOND);

    /*coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &main_server_ep);
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);  
    coap_set_header_uri_path(request, "register/");
    const char msg[] = "actuator";
    coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
    rgb_led_set(RGB_LED_YELLOW);
    COAP_BLOCKING_REQUEST(&main_server_ep, request, client_chunk_handler_registration);
    LOG_INFO("--Registered--\n");*/

    coap_endpoint_parse(ROTATION_SERVER_EP, strlen(ROTATION_SERVER_EP), &rotation_server_ep);
    coap_endpoint_parse(VOLTAGE_SERVER_EP, strlen(VOLTAGE_SERVER_EP), &voltage_server_ep);
    coap_endpoint_parse(PRESSURE_SERVER_EP, strlen(PRESSURE_SERVER_EP), &pressure_server_ep);
    coap_endpoint_parse(VIBRATION_SERVER_EP, strlen(VIBRATION_SERVER_EP), &vibration_server_ep);

    //Observe the resources

    toggle_observation(obs_rotation, &rotation_server_ep, "/rotation");
    toggle_observation(obs_voltage, &voltage_server_ep, "/voltage");
    toggle_observation(obs_pressure, &pressure_server_ep, "/pressure");
    toggle_observation(obs_vibration, &vibration_server_ep, "/vibration");

    

    /*COAP_BLOCKING_REQUEST(&rotation_server_ep, &request[0], client_chunk_handler);
    COAP_BLOCKING_REQUEST(&voltage_server_ep, &request[1], client_chunk_handler);
    COAP_BLOCKING_REQUEST(&pressure_server_ep, &request[2], client_chunk_handler);
    COAP_BLOCKING_REQUEST(&vibration_server_ep, &request[3], client_chunk_handler);*/

    while (1){
      PROCESS_YIELD();
      if(etimer_expired(&et)){
        // make prediction
        // print the prediction

        // prepare vector of float values
        float values[9] = {186.505383,447.676309, 38.942684, 143.116557, 1.0, 0.0, 0.0, 1.0, 0.0};

 
    
        // make prediction
        int prediction = model_predict(values, 9);

        // print the prediction
        printf("Prediction: %d\n", prediction);

        // reset the timer
        etimer_reset(&et);
      }
        
  
    }


    PROCESS_END();
}
