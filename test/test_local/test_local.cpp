#include "unity.h"

#ifdef UNIT_TEST  // IMPORTANT LINE!

void test_test(void) {
    TEST_ASSERT_EQUAL(13, 13);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_test);
    UNITY_END();

    return 0;
}


#endif
