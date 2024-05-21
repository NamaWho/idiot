// creatre a c file util that exposes two functions, one to create a json object and another to parse a json object
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "json.h"

/**
 * @brief Create a json object
 * 
 * @param key
 * @param value
 * @return char*
*/
char* json_create(char* key, char* value) {
    char* json = (char*)malloc(100);
    sprintf(json, "{\"%s\":\"%s\"}", key, value);
    return json;
}

/**
 * @brief Parse a json object
 * 
 * @param json
 * @param key
 * @return char*
*/
char* json_parse(char* json, char* key) {
    char* value = (char*)malloc(100);
    char* start = strstr(json, key);
    if (start == NULL) {
        return NULL;
    }
    start += strlen(key) + 3;
    char* end = strstr(start, "\"");
    strncpy(value, start, end - start);
    return value;
}




