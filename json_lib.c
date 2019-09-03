#include "json_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int parse_file(FILE *file, void **data) {
    int next_char;
    int ret;

    next_char = fgetc(file);

    switch (next_char) {
        case '{':
            *data = (struct JSON *)malloc(sizeof(struct JSON));
            ret = parse_object(file, (struct JSON **)data);
            break;
        case '[':
            *data = (struct Data *) malloc(sizeof(struct Data *));
            //ret = parse_array(file, (struct Data *) data);
            ret = 1;
            break;
        default:
            ret = -1;
    }


    return ret;
}

int parse_object(FILE *file, struct JSON **head) {
    int done = 0;
    int next_char;
    int count = 0;
    char buffer[1000];
    struct JSON * json = *head;

    while (!done) {
        next_char = fgetc(file);

        if (next_char == '"') {
            json->member = (struct Member *) malloc(sizeof(struct Member));
            count = parse_string(file, buffer);

            json->member->key = (char *) malloc(sizeof(char) * count + 1);

            strcpy(json->member->key, buffer);

            next_char = fgetc(file);

            while (next_char == ' ') {
                next_char = fgetc(file);
            }

            if (next_char == ':') {
                json->member->data = (struct Data *) malloc(sizeof(struct Data));
                count = parse_value(file, &json->member->data);

                if (count == -1) {
                    return -1;
                } else if (count == -2) {
                    done = 1;
                }
            } else {
                return -1;
            }

            json->next_member = (struct JSON *) malloc(sizeof(struct JSON));
            json = json->next_member;
            json->next_member= NULL;

        } else if (next_char == ',') {
            continue;
        } else if (next_char == '}') {
            free(json);
            done = 1;
        } else if (next_char == ' ') {
            continue;
        } else if (next_char == '\t') {
            continue;
        } else if (next_char == '\n') {
            continue;
        } else {
            return -1;
        }
    }

    return 0;
}

int parse_string(FILE *file, char *buffer) {
    char next_char;
    int done = 0;
    int ignore_next = 0;
    int buffer_ndx = 0;

    while (!done) {
        next_char = (char) fgetc(file);


        if ((next_char == '\\') && !ignore_next) {
            ignore_next = 1;
        } else if ((next_char == '"') && !ignore_next) {
            buffer[buffer_ndx] = 0;
            done = 1;
        } else {
            ignore_next = 0;
            buffer[buffer_ndx++] = next_char;
        }

    }

    return buffer_ndx;

}

int parse_number(FILE *file, struct Data **data, char firstDigit) {
    char nextChar;
    int count = 0;
    char buffer[1024];
    int done = 0;
    int isFloatingPoint = 0;
    int isEndOfNum = 0;
    int ret = 0;
    buffer[0] = firstDigit;
    count++;

    while (!done && (nextChar = (char) fgetc(file)) != EOF) {
        if ((!isEndOfNum) && isdigit(nextChar)) {
            buffer[count] = nextChar;
            count++;
        } else if (!isEndOfNum && nextChar == '.') {
            isFloatingPoint = 1;
            buffer[count] = nextChar;
            count++;
        } else if (nextChar == ' ') {
            isEndOfNum = 1;
            continue;
        } else if (nextChar == ',' || nextChar == '}') {
            buffer[count] = 0;
            if (isFloatingPoint) {
                (*data)->float_data = (double *) malloc(sizeof(double));
                (*(*data)->float_data) = strtof(buffer, NULL);
                (*data)->type = FLOAT_DATA;
            } else {
                (*data)->int_data = (int *) malloc(sizeof(int));
                (*(*data)->int_data) = (int) strtol(buffer, NULL, 10);
                (*data)->type = INT_DATA;
            }

            done = 1;

            if (nextChar == '}') {
                ret = -2;
            }
            else {
                ret = 0;
            }
        }
        else {
            ret = -1;
            done = 1;
        }
    }

    return ret;
}


int parse_value(FILE *file, struct Data **data) {
    int nextChar;
    int count;
    char buffer[1024];
    nextChar = fgetc(file);

    while (nextChar == ' ') {
        nextChar = fgetc(file);
    }
    if (isdigit(nextChar)) {
        count = parse_number(file, data, (char) nextChar);

        return count;
    } else if (nextChar == '"') {
        count = parse_string(file, buffer);

        (*data)->type = CHAR_DATA;
        (*data)->char_data = (char *) malloc(sizeof(count + 1));

        strcpy((*data)->char_data, buffer);

        return count;
    } else if (nextChar == '{') {
        (*data)->json_data = (struct JSON *)malloc(sizeof(struct JSON));
        (*data)->type = JSON_DATA;
        return parse_object(file, &(*data)->json_data);
    } else if (nextChar == '[') {
        //parse array
        return 0;
    } else {
        return -1;
    }


}

int json_to_string(char ** buffer, struct JSON **json) {
    int ndx = 0;
    int count = 0;
    *buffer = (char *) malloc(2048 * sizeof(char));
    char * str_buffer;
    struct JSON *next_member = *json;

    ndx += sprintf(*buffer + ndx, "{\n");
    while (next_member != NULL) {
        if (next_member->member != NULL && next_member->member->key != NULL) {
            if (next_member->member->data->type == INT_DATA) {
                count = sprintf(*buffer + ndx, "\t\"%s\": %d,\n", next_member->member->key,
                                *next_member->member->data->int_data);
                ndx += count;
            }
            else if (next_member->member->data->type == CHAR_DATA) {
                count = sprintf(*buffer + ndx, "\t\"%s\": \"%s\",\n", next_member->member->key,
                                next_member->member->data->char_data);
                ndx += count;
            }
            else if (next_member->member->data->type == FLOAT_DATA) {
                count = sprintf(*buffer + ndx, "\t\"%s\": %f,\n", next_member->member->key,
                                *next_member->member->data->float_data);
                ndx += count;
            }
            else if (next_member->member->data->type == JSON_DATA) {
                json_to_string(&str_buffer, &next_member->member->data->json_data);
                count = sprintf(*buffer + ndx, "\t\"%s\": \n%s,\n", next_member->member->key, str_buffer);
                ndx += count;
            }
        }
        next_member = next_member->next_member;
    }

    ndx += sprintf(*buffer + ndx, "}");

    return ndx;
}

int free_json(struct JSON ** json) {
    struct JSON * next = (*json)->next_member;

    while (next != NULL) {
        free(*json);
        *json = next;
        next = (*json)->next_member;
    }

    return 0;
}


