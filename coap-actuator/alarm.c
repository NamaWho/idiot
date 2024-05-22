#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"


// include ml model to predict maintenance need

/* ------- Log configuration ------- */
#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
/* ------------------------------- */


/* ------ CoAP resources ------ */
#define SERVER_EP "coap://[fd00::1]:5683"
#define ROTATION_SERVER_EP "coap://[fd00::202:2:2:2]:5683"
#define VOLTAGE_SERVER_EP "coap://[fd00::202:2:2:2]:5683"
#define PRESSURE_SERVER_EP "coap://[fd00::202:2:2:2]:5683"
#define VIBRATION_SERVER_EP "coap://[fd00::202:2:2:2]:5683"

char *service_url = "/telemetry";

#define TOGGLE_INTERVAL 10

PROCESS(alarm_client, "Alarm Actuator Client");
AUTOSTART_PROCESSES(&alarm_client);

/* ------ Machine Configuration ------ */
static uint8_t rotation = 0;
static uint8_t voltage = 0;
static uint8_t pressure = 0;
static uint8_t vibration = 0;

static uint8_t error_one_count = 0;
static uint8_t error_two_count = 0;
static uint8_t error_three_count = 0;
static uint8_t error_four_count = 0;
static uint8_t comp_one_repair = 0;
static uint8_t comp_two_repair = 0;
static uint8_t comp_three_repair = 0;
static uint8_t comp_four_repair = 0;
static uint8_t age = 0;


/* ----------------------------------- */

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
        /* code */
        break;
    case "voltage":
        /* code */
        break;
    case "pressure":
        /* code */
        break;
    case "vibration":  
        /* code */
        break;

    default:
        break;
    }
}

void client_chunk_handler_registration(coap_message_t *response)
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


PROCESS_THREAD(alarm_client, ev, data)
{
    static coap_endpoint_t main_server_ep;
    static coap_endpoint_t rotation_server_ep;
    static coap_endpoint_t voltage_server_ep;
    static coap_endpoint_t pressure_server_ep;
    static coap_endpoint_t vibration_server_ep;
    static coap_message_t request[4];

    PROCESS_BEGIN();

    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &main_server_ep);
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);  
    coap_set_header_uri_path(request, "register/");
    const char msg[] = "actuator";
    coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
    rgb_led_set(RGB_LED_YELLOW);
    COAP_BLOCKING_REQUEST(&main_server_ep, request, client_chunk_handler_registration);
    LOG_INFO("--Registered--\n");

    coap_endpoint_parse(ROTATION_SERVER_EP, strlen(ROTATION_SERVER_EP), &rotation_server_ep);
    coap_endpoint_parse(VOLTAGE_SERVER_EP, strlen(VOLTAGE_SERVER_EP), &voltage_server_ep);
    coap_endpoint_parse(PRESSURE_SERVER_EP, strlen(PRESSURE_SERVER_EP), &pressure_server_ep);
    coap_endpoint_parse(VIBRATION_SERVER_EP, strlen(VIBRATION_SERVER_EP), &vibration_server_ep);

    for (int i = 0; i < 4; i++) {
        coap_init_message(&request[i], COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(&request[i], service_url);
        coap_set_header_observe(&request[i], 0);
    }

    COAP_BLOCKING_REQUEST(&rotation_server_ep, &request[0], client_chunk_handler);
    COAP_BLOCKING_REQUEST(&voltage_server_ep, &request[1], client_chunk_handler);
    COAP_BLOCKING_REQUEST(&pressure_server_ep, &request[2], client_chunk_handler);
    COAP_BLOCKING_REQUEST(&vibration_server_ep, &request[3], client_chunk_handler);

    while (1)
        PROCESS_YIELD();

    PROCESS_END();
}
