#ifndef JSON_LIB_LIBRARY_H
#define JSON_LIB_LIBRARY_H

#include <stdio.h>

#define INT_DATA 0
#define FLOAT_DATA 1
#define CHAR_DATA 2
#define JSON_DATA 3
#define ARRAY_DATA 4

struct Data {
    __uint8_t type;
    int * int_data;
    double * float_data;
    char * char_data;
    struct JSON * json_data;
    struct Data * array;
};

struct Member {
    char * key;
    struct Data * data;
};

struct JSON {
    struct Member * member;
    struct JSON * next_member;
};

int parse_file(FILE *, void **);

int parse_object(FILE *, struct JSON **);

int parse_array(FILE *, struct Data **);

int parse_value(FILE *, struct Data **);

int parse_number(FILE *, struct Data **, char firstDigit);

int parse_string(FILE *, char *);

int json_to_string(char **, struct JSON **);

int free_json(struct JSON **);


#endif