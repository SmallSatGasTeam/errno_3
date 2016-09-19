#include <iostream>
#include "sensor.h"
#include <random>

float pseudoReading()
{
  static std::random_device rd;
  static std::mt19937 engine(rd());
  std::uniform_int_distribution<> die(1, 9);
  return die(engine) * 23.91;
}

int main()
{
  for(int i = 0; i < 100; ++i)
  {
    auto reading = pseudoReading();
    std::cout << "Reading: " << reading << ", median: ";
    std::cout << getMedian(reading, 10) << std::endl;
  }
}
