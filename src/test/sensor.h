#ifndef SENSORS_H
#define SENSORS_H

#include "list.h"

// ------------ Data Validation ------------ //

void swap(float & a, float & b)
{
  auto temp = a;
  a = b;
  b = temp;
}

int partition(LinkedList & list, int start, int end, int pivot)
{
  swap(list[pivot], list[end - 1]);
  int firstGreater = start;
  for (int i = start; i < end - 1; ++i)
  {
    if (list[i] <= list[end - 1])
    {
      swap(list[i], list[firstGreater]);
      ++firstGreater;
    }
  }
  swap(list[end - 1], list[firstGreater]);
  return firstGreater;
}

int getPivot(LinkedList & list, int start, int end)
{
  auto first = list[start];
  auto last = list[end - 1];
  auto mid = list[(start + end) / 2];
  if ((first <= last && last <= mid) || (mid <= last && last <= first)) return end - 1;
  else if ((last <= first && first <= mid) || (mid <= first && first <= last)) return start;
  else if ((first <= mid && mid <= last) || (last <= mid && mid <= first)) return (start + end)/2;
  else return mid;
}

void quickSort(LinkedList & list, int start, int end)
{
  if (end - start <= 1) return;
  auto pivot = getPivot(list, start, end);
  pivot = partition(list, start, end, pivot);

  quickSort(list, start, pivot);
  quickSort(list, pivot, end);
}

float getMedian(float reading, const int RANGE)
{
  static LinkedList list;
  auto median = 0;

  list.push_back(reading);
  auto size = list.size();

  if (size >= RANGE)
  {
    auto tempList = list;
    quickSort(tempList, 0, size); 
    median = tempList[ size / 2 ];

    list.pop_front();
  }
  
  return median;
}

#endif

