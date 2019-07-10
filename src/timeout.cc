#include "timeout.hh"
#include <time.h>
#include <functional>
#include <iostream>
std::function<bool()> generate_timer(double timeout)
{
  int start_t = clock();
  return [start_t, timeout](){
    int end_t = clock();
    double diff = (double)(end_t-start_t)/CLOCKS_PER_SEC;
    return (diff < timeout);
  };
}