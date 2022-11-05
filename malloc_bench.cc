#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <random>
#include <cassert>
#include <fstream>
#include <cstring>
#include <unistd.h>

#include <gflags/gflags.h>

#include "timer.h"

#define VMRSS_LINE 21

// define gflags
DEFINE_bool(WITH_GLIBC_MALLOC, true, "test glibc malloc. Default: true");
DEFINE_uint64(malloc_per_thread, 4096, "malloc requests num per threads. Default: 4096");
DEFINE_uint64(thread_num, 1, "threads number. Default: 1");
DEFINE_uint64(min_size, 1, "min size in one malloc request. Default: 1 (1B)");
DEFINE_uint64(max_size, 2097152, "min size in one malloc request. Default: 2097152 (2MB)");
DEFINE_string(time_units, "micro", "time units used in benchmark: sec, milli, micro, nano. Default: micro");

// gflags validator
static bool ValidateSize(const char* flagname, uint64_t value) {
  if (value == 0) 
    return false;
  return true;
}
static bool ValidateTimeUints(const char* flagname, const std::string& value) {
  if (value == "sec" || value == "milli" || value == "micro" || value == "nano") {
    return true;
  }
  return false;
}
DEFINE_validator(min_size, &ValidateSize);
DEFINE_validator(max_size, &ValidateSize);
DEFINE_validator(time_units, &ValidateTimeUints);

// malloc
void do_malloc() {
  void* ptrs[FLAGS_malloc_per_thread];
  for (uint64_t i = 0; i < FLAGS_malloc_per_thread; i++) {
    uint64_t size = rand() % (FLAGS_max_size - FLAGS_min_size + 1) + FLAGS_min_size;
    ptrs[i] = malloc(size);
  }
  return ;
}


// Get the memory in /proc/%d/status
void get_phy_mem(const pid_t p) {
  std::string file_name = "/proc/" + std::to_string(p) + "/status";
  std::ifstream fd(file_name, std::ios::in);

  char s[256];
  char tmp[256];
  uint32_t mem_size;
  while(!fd.eof()) {
    fd.getline(s, sizeof(s), '\n');
    if(strncmp(s, "VmSize", 6) == 0) {
      sscanf(s, "%s %d", tmp, &mem_size);
      std::cout << "Virtual Mem: " << mem_size << std::endl;
    } else if(strncmp(s, "VmRSS", 5) == 0) {
      sscanf(s, "%s %d", tmp, &mem_size);
      std::cout << "Physical Mem: " << mem_size << std::endl;
      break;
    }
  }
  fd.close();
}

timer::time_units select_time_units(const std::string time_units) {
  if (time_units == "sec")
    return timer::time_units::second;
  if (time_units == "milli")
    return timer::time_units::millisecond;
  if (time_units == "micro")
    return timer::time_units::microsecond;
  if (time_units == "nano")
    return timer::time_units::nanosecond;
}

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_min_size > FLAGS_max_size) {
    std::cout << "error: min_size is greater than max_size" << std::endl;
    exit(1);
  }

  timer t;
  auto tu = select_time_units(FLAGS_time_units);
  std::thread* thread_ptrs[FLAGS_thread_num];

  t.start();
  for(uint64_t i = 0; i < FLAGS_thread_num; i++) {
    thread_ptrs[i] = new std::thread(do_malloc);
  }
  for(uint64_t i = 0; i < FLAGS_thread_num; i++) {
    thread_ptrs[i]->join();
  }
  t.end();

  get_phy_mem(getpid());
  std::cout << "Wall time used: " << t.get_wall_time(tu) << std::endl;
  std::cout << "CPU time used: " << t.get_cpu_time(tu) << std::endl;

  return 0;
}