/**
 * DumbJSONLib
 *
 * A dumb json library written in C
 */

//TODO
//Add tests dir (move main to there)
//Commit to github
//Add array parsing
//Switch from buffer io to older style file io (so we can read from sockets)


#include "../json_lib.h"
#include "CuTest.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void test_init(CuTest * tc) {
    struct Data data[100];
    struct Member member[100];
    unsigned char strings[1000];
    struct JSON json;

    int ret = json_init(&json, member, 100, data, 100, strings, 1000);

    CuAssertIntEquals(tc, ret,  0);

}

void test_parse_string(CuTest * tc) {
    struct Data data[100];
    struct Member member[100];
    unsigned char strings[1000];
    struct JSON json;
    int ndx = 0;

    json_init(&json, member, 100, data, 100, strings, 1000);

    char * buffer = "this is a test";

    unsigned char * str_ptr = parse_string(&json, &ndx, (unsigned char * )buffer, (int)strlen(buffer));

    CuAssertStrEquals(tc, buffer, (char *)json.strings);
    CuAssertPtrEquals(tc, json.string_stack_ptr, str_ptr+strlen(buffer));
}

void test_parse_json_string(CuTest * tc) {
    struct Data data[100];
    struct Member member[100];
    unsigned char strings[1000];
    struct JSON json;

    json_init(&json, member, 100, data, 100, strings, 1000);

    char * buffer = "{\"this\" : \"is a test\", \"a good\": \"test\"}";

    int ret = parse_json(&json, (unsigned char *)buffer, (int)strlen(buffer));

    CuAssertIntEquals(tc, strlen(buffer)-1, ret);

    CuAssertStrEquals(tc, "this", (char *)json.members[0].key);
    CuAssertStrEquals(tc, "test", (char *)json.members[1].data->data.char_data);
}

void test_parse_json_number(CuTest * tc) {
    struct Data data[100];
    struct Member member[100];
    unsigned char strings[1000];
    struct JSON json;

    json_init(&json, member, 100, data, 100, strings, 1000);

    char * buffer = "{\"this\" : 12345, \"test\" : -5.548}";

    int ret = parse_json(&json, (unsigned char *)buffer, (int)strlen(buffer));

    CuAssertIntEquals(tc, strlen(buffer)-1, ret);
    CuAssertIntEquals(tc, 12345, json.members[0].data->data.int_data);
    CuAssertDblEquals(tc, -5.548f, json.members[1].data->data.float_data, 0.001);
}

void test_parse_json_list(CuTest * tc) {
    struct Data data[100];
    struct Member member[100];
    unsigned char strings[1000];
    struct JSON json;

    json_init(&json, member, 100, data, 100, strings, 1000);

    char * buffer = "{\"this\" : [\"is a test\", \"a good\", 1235, 5.0, \"test\"]}";

    int ret = parse_json(&json, (unsigned char *)buffer, (int)strlen(buffer));

    CuAssertIntEquals(tc, strlen(buffer)-1, ret);

    CuAssertStrEquals(tc, "this", (char *)json.members[0].key);
    CuAssertStrEquals(tc, "is a test", (char *)json.members[0].data->data.array_data[0].data.char_data);
    CuAssertIntEquals(tc, 1235, json.members[0].data->data.array_data[2].data.int_data);
}

void test_parse_number_int(CuTest * tc) {
    struct Data data[100];
    struct Member member[100];
    unsigned char strings[1000];
    struct JSON json;
    int ndx = 0;
    unsigned char * test_vector[] = {"5 ", "-1,", "2048 ", "-152 "};

    json_init(&json, member, 100, data, 100, strings, 1000);

    for (int i = 0; i < sizeof(test_vector)/sizeof(test_vector[0]); i++) {
        unsigned char * str = test_vector[i];
        int value = (int)strtol((char *)str, NULL, 10);
        ndx = 0;

        struct Data * data_ptr = parse_number(&json, &ndx, str, (int)strlen((char *)str));

        CuAssertIntEquals(tc, value, data_ptr->data.int_data);
    }
}

void test_parse_number_float(CuTest * tc) {
    struct Data data[100];
    struct Member member[100];
    unsigned char strings[1000];
    struct JSON json;
    int ndx = 0;
    unsigned char * test_vector[] = {"5.0, ", "-1.0 ", "2048.1546546 ", "-150.92}"};

    json_init(&json, member, 100, data, 100, strings, 1000);

    for (int i = 0; i < sizeof(test_vector)/sizeof(test_vector[0]); i++) {
        unsigned char * str = test_vector[i];
        float value = strtof((char *)str, NULL);
        ndx = 0;

        struct Data * data_ptr = parse_number(&json, &ndx, str, (int)strlen((char *)str));

        CuAssertDblEquals(tc, value, data_ptr->data.float_data, 0.0001);
    }
}

void test_parse_list(CuTest * tc) {
    struct Data data[100];
    struct Member member[100];
    unsigned char strings[1000];
    struct JSON json;
    int ndx = 0;
    json_init(&json, member, 100, data, 100, strings, 1000);

    unsigned char * list = "\"this is\", 55, 5.0, -1, \"a good test\", 55]";

    struct Data * data_ptr = parse_list(&json, &ndx, list, (int)strlen((char *) list));

    CuAssertPtrNotNull(tc, data_ptr);
    CuAssertIntEquals(tc, 6, data_ptr->array_length);
    CuAssertIntEquals(tc, 55, data_ptr->data.array_data[1].data.int_data);
    CuAssertStrEquals(tc, "a good test", (char *)data_ptr->data.array_data[4].data.char_data);
}


CuSuite* test_suite() {
    CuSuite *suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_init);
    SUITE_ADD_TEST(suite, test_parse_string);
    SUITE_ADD_TEST(suite, test_parse_number_int);
    SUITE_ADD_TEST(suite, test_parse_number_float);
    SUITE_ADD_TEST(suite, test_parse_list);
    SUITE_ADD_TEST(suite, test_parse_json_string);
    SUITE_ADD_TEST(suite, test_parse_json_number);
    SUITE_ADD_TEST(suite, test_parse_json_list);
    return suite;
}



void all_tests()
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, test_suite());
    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main()
{
    all_tests();
    return 0;
}

