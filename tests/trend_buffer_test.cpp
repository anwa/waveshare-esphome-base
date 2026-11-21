// Host test for waveshare_ui::TrendBuffer. Build and run with any C++17
// compiler, for example:
//   g++ -std=c++17 -I components tests/trend_buffer_test.cpp -o trend_buffer_test
//   ./trend_buffer_test

#include <cassert>
#include <cmath>
#include <cstdio>

#include "waveshare_ui/trend_buffer.h"

using Buffer = waveshare_ui::TrendBuffer<5, 2, 10000>;

static Buffer::Sample sample(float a, float b) { return {{a, b}}; }

int main() {
  Buffer buffer;

  // An empty buffer has no valid point.
  for (size_t p = 0; p < Buffer::points; ++p) assert(std::isnan(buffer.at(p, 0)));

  // Samples fill the timeline from the newest end.
  buffer.push(sample(1, 10), 0);
  buffer.push(sample(2, 20), 10000);
  assert(buffer.at(4, 0) == 2 && buffer.at(4, 1) == 20);
  assert(buffer.at(3, 0) == 1);
  assert(std::isnan(buffer.at(2, 0)));

  // A missed interval becomes a gap instead of an interpolated value.
  buffer.push(sample(4, 40), 30000);
  assert(buffer.at(4, 0) == 4);
  assert(std::isnan(buffer.at(3, 0)));
  assert(buffer.at(2, 0) == 2);

  // Overflowing the ring keeps only the newest points.
  buffer.push(sample(5, 50), 40000);
  buffer.push(sample(6, 60), 50000);
  buffer.push(sample(7, 70), 60000);
  assert(buffer.at(4, 0) == 7 && buffer.at(1, 0) == 4);
  assert(std::isnan(buffer.at(0, 0)));

  // A long outage clears the whole visible window.
  buffer.push(sample(8, 80), 200000);
  assert(buffer.at(4, 0) == 8);
  for (size_t p = 0; p < 4; ++p) assert(std::isnan(buffer.at(p, 0)));

  // millis() rollover is handled by unsigned subtraction.
  Buffer rollover;
  rollover.push(sample(1, 1), 0xFFFFFFFFu - 5000u);
  rollover.push(sample(2, 2), 4999u);
  assert(rollover.at(4, 0) == 2 && rollover.at(3, 0) == 1);

  // Out-of-range access returns NAN.
  assert(std::isnan(buffer.at(Buffer::points, 0)));
  assert(std::isnan(buffer.at(0, Buffer::channels)));

  std::puts("trend_buffer_test: all checks passed");
  return 0;
}
