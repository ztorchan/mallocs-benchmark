#ifndef TIMER_H_
#define TIMER_H_

#include <ctime>
#include <chrono>
#include <iostream>

class timer {
  public:
    enum time_units {
      second,
      millisecond,
      microsecond,
      nanosecond,
    };

    timer() : cpu_time_start_(), cpu_time_end_(), wall_time_start_(), wall_time_end_() {};

    void start();
    void end();
    uint64_t get_cpu_time(const time_units);
    uint64_t get_wall_time(const time_units);

  private:
    bool timing = false;

    clock_t cpu_time_start_;
    clock_t cpu_time_end_;
    std::chrono::time_point<std::chrono::steady_clock> wall_time_start_;
    std::chrono::time_point<std::chrono::steady_clock> wall_time_end_;
};


void timer::start() {
  timing = true;
  cpu_time_start_ = clock();
  wall_time_start_ = std::chrono::steady_clock::now();
}

void timer::end() {
  if(timing) {
    cpu_time_end_ = clock();
    wall_time_end_ = std::chrono::steady_clock::now();
    timing = false;
  }
}

uint64_t timer::get_cpu_time(const time_units tu) {
  uint64_t tu_ratio;
  switch (tu)
  {
  case time_units::second:
    tu_ratio = 1;
    break;
  case time_units::millisecond:
    tu_ratio = 1000;
    break;
  case time_units::microsecond:
    tu_ratio = 1000000;
    break;
  case time_units::nanosecond:
    tu_ratio = 1000000000;
    break;
  }
  return (cpu_time_end_ - cpu_time_start_) * ((double)tu_ratio / CLOCKS_PER_SEC);
}

uint64_t timer::get_wall_time(const time_units tu) {
  switch(tu) {
    case time_units::second:
      return std::chrono::duration_cast<std::chrono::seconds>(wall_time_end_ - wall_time_start_).count();
    case time_units::millisecond:
      return std::chrono::duration_cast<std::chrono::milliseconds>(wall_time_end_ - wall_time_start_).count();
    case time_units::microsecond:
      return std::chrono::duration_cast<std::chrono::microseconds>(wall_time_end_ - wall_time_start_).count();
    case time_units::nanosecond:
      return std::chrono::duration_cast<std::chrono::nanoseconds>(wall_time_end_ - wall_time_start_).count();
  }
}

#endif