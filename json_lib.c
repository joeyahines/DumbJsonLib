#include "json_lib.h"

int json_init(struct JSON * json_ptr, struct Member * member_array, int member_array_size, struct Data * data_array, int data_array_size, unsigned char * string_array, int string_array_size) {
    json_ptr->data = data_array;
    json_ptr->members = member_array;
    json_ptr->strings = string_array;

    json_ptr->data_stack_ptr = data_array;
    json_ptr->member_stack_ptr = member_array;
    json_ptr->string_stack_ptr = string_array;

    json_ptr->members_size = member_array_size;
    json_ptr->data_size = data_array_size;
    json_ptr->strings_size = string_array_size;

    return 0;
}

int parse_json(struct JSON * json_ptr, const unsigned char * buffer, int buffer_size) {
    int buffer_ndx = 0;
    int parse_value_flag = 0;
    int ret = -1;
    unsigned char * str_ptr;
    struct Data * data;

    if (buffer[buffer_ndx++] == '{') {
        while (buffer_ndx < buffer_size) {
            if (buffer[buffer_ndx] ==' ' || buffer[buffer_ndx] == '\n' || buffer[buffer_ndx] == '\t') {
                buffer_ndx++;
            }
            else if (buffer[buffer_ndx] == '}') {
                ret = buffer_ndx;
                goto end;
            } else if (!parse_value_flag) {
                if (buffer[buffer_ndx] == '"') {
                    buffer_ndx++;
                    str_ptr = parse_string(json_ptr, &buffer_ndx, buffer, buffer_size);
                    json_ptr->member_stack_ptr->key = str_ptr;
                    parse_value_flag = 1;
                }
                else if (buffer[buffer_ndx] == ':') {
                    goto end;
                }
                else {
                    buffer_ndx++;
                }
            } else {
                if (buffer[buffer_ndx] != ':') {
                    data = parse_value(json_ptr, &buffer_ndx, buffer, buffer_size);
                    if (data != 0) {
                        parse_value_flag = 0;
                        json_ptr->member_stack_ptr->data = data;
                        json_ptr->member_stack_ptr++;
                    } else {
                        goto end;
                    }
                }
                else {
                    buffer_ndx++;
                }
            }
        }
    }

    end:
    return ret;
}

struct Data * parse_value(struct JSON * json_ptr, int * ndx, const unsigned char * buffer, int buffer_size) {
    unsigned char * str_ptr;
    struct Data * data_ptr = 0;

    if (buffer[*ndx] == '"') {
        (*ndx)++;
        str_ptr = parse_string(json_ptr, ndx, buffer, buffer_size);
        json_ptr->data_stack_ptr->type = CHAR_DATA;
        json_ptr->data_stack_ptr->data.char_data = str_ptr;

        data_ptr = json_ptr->data_stack_ptr;

        json_ptr->data_stack_ptr++;
    }
    else if (buffer[*ndx] == '-' || isdigit((char) buffer[*ndx])) {
        data_ptr = parse_number(json_ptr, ndx, buffer, buffer_size);
    } else if (buffer[*ndx] == '{') {
        // TODO added nested JSON support
        data_ptr = 0;
    } else if (buffer[*ndx] == '[') {
        // parse list
        data_ptr = 0;
    }
    else {
        (*ndx)++;
    }

    return data_ptr;
}



unsigned char * parse_string(struct JSON * json_ptr, int * ndx, const unsigned char * buffer, int buffer_size) {
    int done = 0;
    int ignore_next = 0;
    char next_char;
    unsigned char * string_ptr = json_ptr->string_stack_ptr;
    while (*ndx < buffer_size && !done) {
        next_char = (char)buffer[*ndx];

        if ((next_char == '\\') && !ignore_next) {
            ignore_next = 1;
        } else if ((next_char == '\"') && !ignore_next) {
            *json_ptr->string_stack_ptr = 0;
            json_ptr->string_stack_ptr++;
            done = 1;
        } else {
            ignore_next = 0;
            *json_ptr->string_stack_ptr = next_char;
            json_ptr->string_stack_ptr++;
        }
        (*ndx)++;
    }
    return string_ptr;
}

struct Data * parse_number(struct JSON * json_ptr, int * ndx, const unsigned char * buffer, int buffer_size) {
    char nextChar;
    int done = 0;
    int isFloatingPoint = 0;
    int negative = 1;
    int integral = 0;
    float fractional = 0.0f;
    float fractional_place = 0.1f;
    struct Data * data_ptr = 0;

    while (!done && (*ndx < buffer_size)) {
        nextChar = (char)buffer[*ndx];
        if (isdigit(nextChar) && !isFloatingPoint) {
            integral = (integral * 10) + (nextChar - '0');
        }
        else if (isdigit(nextChar) && isFloatingPoint) {
            fractional += (((float)((nextChar - '0')) * fractional_place));
            fractional_place *= 0.1f;
        }
        else if (nextChar == '.') {
            isFloatingPoint = 1;
        }
        else if (nextChar == '-') {
            negative = -1;
        }
        else if (nextChar == ',' || nextChar == '}' || nextChar == ' ') {
            (*ndx)--;
            done = 1;
            continue;
        }
        else {
            done = -1;
        }
        (*ndx)++;
    }

    if (done == 1) {

        if (isFloatingPoint) {
            json_ptr->data_stack_ptr->type = FLOAT_DATA;
            json_ptr->data_stack_ptr->data.float_data = ((float)integral + fractional) * (float)negative;
            data_ptr = json_ptr->data_stack_ptr;
        }
        else {
            json_ptr->data_stack_ptr->type = INT_DATA;
            json_ptr->data_stack_ptr->data.int_data = integral * negative;
            data_ptr = json_ptr->data_stack_ptr;
        }
        json_ptr->data_stack_ptr++;
    }

    return data_ptr;
}

int isdigit(char c) {
    return c >= '0' && c <= '9';
}


