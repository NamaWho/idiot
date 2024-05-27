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
char* json_create_string(const char* key,const char* value) {
    size_t length = strlen(key) + strlen(value) + 7;
    char* json = (char*)malloc(length);
    if (json == NULL) {
        return NULL;
    }
    snprintf(json, length, "{\"%s\":\"%s\"}", key, value);
    return json;
}

/**
 * @brief Create a JSON object with a numerical value.
 * 
 * @param key The key for the JSON object.
 * @param value The numerical value for the JSON object.
 * @return char* The created JSON object as a string.
 */
char* json_create_number(const char* key, double value) {
    size_t length = strlen(key) + 30; // Allow space for the key and a large number
    char* json = (char*)malloc(length);
    if (json == NULL) {
        return NULL;
    }
    snprintf(json, length, "{\"%s\":%.2f}", key, value);
    return json;
}


/**
 * @brief Parse a value from a JSON object string by key.
 * 
 * @param json The JSON object string.
 * @param key The key to find the value for.
 * @return char* The value associated with the key, or NULL if not found.
 */
char* json_parse_string(const char* json, const char* key) {
    const char* key_start = strstr(json, key);
    if (key_start == NULL) {
        return NULL;
    }
    key_start += strlen(key) + 3;
    const char* value_end = strchr(key_start, '"');
    if (value_end == NULL) {
        return NULL;
    }

    size_t value_length = value_end - key_start;
    char* value = (char*)malloc(value_length + 1);
    if (value == NULL) {
        return NULL;
    }

    strncpy(value, key_start, value_length);
    value[value_length] = '\0';
    return value;
}

/**
 * @brief Parse a numerical value from a JSON object string by key.
 * 
 * @param json The JSON object string.
 * @param key The key to find the numerical value for.
 * @return int The numerical value associated with the key, or -1 if not found.
 */
double json_parse_number(const char* json, const char* key) {
    const char* key_start = strstr(json, key);
    if (key_start == NULL) {
        return -1;
    }
    key_start += strlen(key) + 2;
    double value;
    sscanf(key_start, "%lf", &value);
    return value;
}

/**
 * @brief Parse a JSON object from a JSON object string by key.
 * 
 * @param json The JSON object string.
 * @param key The key to find the JSON object for.
 * @return char* The JSON object associated with the key, or NULL if not found.
 */
char* json_parse_object(const char* json, const char* key) {
    const char* key_start = strstr(json, key);
    if (key_start == NULL) {
        return NULL;
    }
    key_start += strlen(key) + 2;
    const char* value_end = key_start;
    int brace_count = 1;
    while (brace_count > 0) {
        if (*value_end == '{') {
            brace_count++;
        } else if (*value_end == '}') {
            brace_count--;
        }
        value_end++;
    }

    size_t value_length = value_end - key_start;
    char* value = (char*)malloc(value_length + 1);
    if (value == NULL) {
        return NULL;
    }

    strncpy(value, key_start, value_length);
    value[value_length] = '\0';
    return value;
}

/**
 * @brief Free the memory allocated for a JSON string.
 * 
 * @param json The JSON string to free.
 */
void json_free(char* json) {
    free(json);
}