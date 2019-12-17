#ifndef JSON_LIB_LIBRARY_H
#define JSON_LIB_LIBRARY_H

enum DataType {INT_DATA, FLOAT_DATA, CHAR_DATA, JSON_DATA, ARRAY_DATA};

union DataUnion {
    int int_data;
    double float_data;
    unsigned char * char_data;
    struct Data * array_data;
};

struct Data {
    enum DataType type;
    union DataUnion data;
    int array_length;
};

struct Member {
    unsigned char * key;
    struct Data * data;
};


struct JSON {
    struct Member * members;
    struct Member * member_stack_ptr;
    int members_size;
    struct Data * data;
    struct Data * data_stack_ptr;
    int data_size;
    unsigned char * strings;
    unsigned char * string_stack_ptr;
    int strings_size;
};

int json_init(struct JSON *, struct Member *,  int, struct Data *, int, unsigned char *, int);
int parse_json(struct JSON *, const unsigned char *, int);
unsigned char * parse_string(struct JSON *, int *, const unsigned char *, int);
struct Data * parse_number(struct JSON *, int *, const unsigned char *, int);
struct Data * parse_list(struct JSON *, int *, const unsigned char *, int);
struct Data * parse_value(struct JSON *, int *, const unsigned char *, int, int);
int is_digit(char);
int is_white_space(char);

#endif