#include <time.h>
#include <functional>

/**
 * Returns a function that returns true if the timer has been
 * generated since time seconds, with time < timeout
 * ex:
 *  auto timer = generate_timer(3);
 *  while(timer())
 *    continue;
 *  std::cout << "3 seconds have passed" << std::endl;
 */
std::function<bool()> generate_timer(int timeout);