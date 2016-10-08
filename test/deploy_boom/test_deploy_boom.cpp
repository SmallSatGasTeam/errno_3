const float DEPLOY_MIN_PRESSURE = 5.0;
const float DEPLOY_MAX_PRESSURE = 10.0;

// Mock stream library
class Stream{
public:
  Stream(int c, char r):count(c), response(r){}
  bool available(){
    if(count > 0) return true;
  }
  char read(){
    if(count > 0){
      count--;
      return response;
    }
    return 0;
  }

  int count;
  char response;
};

#include <BoomDeploy.hpp>
#include <unity.h>
// #include <iostream>

#ifdef UNIT_TEST


// Method signature
// bool shouldDeployBoom(bool deployed, bool initialized, bool confirmed, float pressure)
void test_shouldDeployBoom(void){
bool results[24] = {false};

const float below_min = 1.0;
const float in_range = 7.0;
const float above_max = 15.0;

// Truth table for each configuration this function can be called with. This gives
// us 24 possible outcomes. for each boolean value there is two possibilites, true/false,
// also for our pressure it could be in 1 of 3 different categories. below target range,
// in target range, or above target range.
// This gives us 2 * 2 * 2 * 3 or 24 different possibilites. This tests all of them.

// When pressure is below minimum auto deploy

  // All bools either true or false
results[0] = shouldDeployBoom(true, true, true, below_min);
TEST_ASSERT_TRUE(results[0]);
results[1] = shouldDeployBoom(false, false, false, below_min);
TEST_ASSERT_FALSE(results[1]);

  // bools 1 false, 2 true
results[2] = shouldDeployBoom(false, true, true, below_min);
TEST_ASSERT_TRUE(results[2]);
results[3] = shouldDeployBoom(true, false, true, below_min);
TEST_ASSERT_FALSE(results[3]);
results[4] = shouldDeployBoom(true, true, false, below_min);
TEST_ASSERT_FALSE(results[4]);

  // bools are 1 true, 2 false
results[5] = shouldDeployBoom(true, false, false, below_min);
TEST_ASSERT_FALSE(results[5]);
results[6] = shouldDeployBoom(false, true, false, below_min);
TEST_ASSERT_FALSE(results[6]);
results[7] = shouldDeployBoom(false, false, true, below_min);
TEST_ASSERT_FALSE(results[7])

// ----- When pressure is in range

  //follows same pattern as above
results[8] = shouldDeployBoom(true, true, true, in_range);
TEST_ASSERT_TRUE(results[8]);
results[9] = shouldDeployBoom(false, false, false, in_range);
TEST_ASSERT_TRUE(results[9]);

results[10] = shouldDeployBoom(false, true, true, in_range);
TEST_ASSERT_TRUE(results[10]);
results[11] = shouldDeployBoom(true, false, true, in_range);
TEST_ASSERT_FALSE(results[11]);
results[12] = shouldDeployBoom(true, true, false, in_range);
TEST_ASSERT_FALSE(results[12]);

results[13] = shouldDeployBoom(true, false, false, in_range);
TEST_ASSERT_FALSE(results[13]);
results[14] = shouldDeployBoom(false, true, false, in_range);
TEST_ASSERT_TRUE(results[14]);
results[15] = shouldDeployBoom(false, false, true, in_range);
TEST_ASSERT_TRUE(results[15]);

// --- When pressure is above range

  //follows same pattern as above
results[16] = shouldDeployBoom(true, true, true, above_max);
TEST_ASSERT_TRUE(results[16]);
results[17] = shouldDeployBoom(false, false, false, above_max);
TEST_ASSERT_FALSE(results[17]);

results[18] = shouldDeployBoom(false, true, true, above_max);
TEST_ASSERT_TRUE(results[18]);
results[19] = shouldDeployBoom(true, false, true, above_max);
TEST_ASSERT_FALSE(results[19]);
results[20] = shouldDeployBoom(true, true, false, above_max);
TEST_ASSERT_FALSE(results[20]);

results[21] = shouldDeployBoom(true, false, false, above_max);
TEST_ASSERT_FALSE(results[21]);
results[22] = shouldDeployBoom(false, true, false, above_max);
TEST_ASSERT_FALSE(results[22]);
results[23] = shouldDeployBoom(false, false, true, above_max);
TEST_ASSERT_FALSE(results[23]);
}

void test_getMessage(void){
  Stream a(10, 1), b(3, 2);
  Stream* streams[] = {&a, &b, (Stream*)NULL};

  char result = getMessage(streams);
  TEST_ASSERT_EQUAL(result, 2);
  TEST_ASSERT_FALSE(a.available()); // Clears out rest of buffer
  TEST_ASSERT_FALSE(b.available());
}

void test_getMessage__single(void){
  Stream a(10, 1);
  Stream* streams[] = {&a, (Stream*)NULL};

  char result = getMessage(streams);
  TEST_ASSERT_EQUAL(result, 1);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_shouldDeployBoom);
    RUN_TEST(test_getMessage);
    RUN_TEST(test_getMessage__single);
    UNITY_END();
    return 0;
}


#endif
