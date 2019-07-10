#include "timeout.hh"

std::function<bool()> generate_timer(int timeout)
{
  time_t start_t;
  time(&start_t);
  return [start_t, timeout](){
    time_t end_t;
    time(&end_t);
    return (difftime(end_t, start_t) < timeout);
  };
}