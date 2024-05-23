#ifndef JSON_H
#define JSON_H

char* json_create_string(const char* key, const char* value);
char* json_create_number(const char* key, double value);
char* json_parse_string(const char* json, const char* key);
double json_parse_number(const char* json, const char* key);
void json_free(char* json);

#endif