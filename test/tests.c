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

void test_parse_file(CuTest* tc) {
    FILE * file;
    int count = 0;
    struct JSON * test = NULL;

    file = fopen("../test/test.txt", "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    count = parse_file(file, (void *)&test);
    CuAssertIntEquals(tc, count, 0);

    fclose(file);

    free_json(&test);
}

void test_print_json(CuTest* tc) {
    FILE * file;
    char * buffer = NULL;
    int count = 0;

    struct JSON * test = NULL;

    file = fopen("../test/test.txt", "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    count = parse_file(file, (void *)&test);
    CuAssertIntEquals(tc, count, 0);


    count = json_to_string(&buffer, (void *)&test);
    printf("Size: %d: \n%s\n", count, buffer);

    CuAssertIntEquals(tc, count, 105);

    fclose(file);
    free_json(&test);
    free(buffer);
}

CuSuite* test_suite() {
    CuSuite *suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_parse_file);
    SUITE_ADD_TEST(suite, test_print_json);
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

