// #include <MedianFilter.h>
// #include "CoolSatBaro.h"
#include <unity.h>

#ifdef UNIT_TEST  // IMPORTANT LINE!

void test_test_1(void) {
    TEST_ASSERT_EQUAL(13, 13);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_test_1);
    UNITY_END();

    return 0;
}


#endif
