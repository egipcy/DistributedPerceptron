#pragma once
#include <time.h>
#include <functional>

#define _ELECTION_ANSWER_TIMEOUT_ 5 //seconds
#define _MASTER_WAIT_TIMEOUT_ 5 //seconds

/**
 * Returns a function that returns true if the timer has been
 * generated since time seconds, with time < timeout
 * ex:
 *  auto timer = generate_timer(3);
 *  while(timer())
 *    continue;
 *  std::cout << "3 seconds have passed" << std::endl;
 */
std::function<bool()> generate_timer(double timeout);