#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace waveshare_ui {

// RAM-only ring buffer for trend charts with a fixed sample cadence.
//
// POINTS samples of CHANNELS values each, one sample per INTERVAL_MS. When
// push() is called after more than one interval, the missed intervals are
// filled with NAN so a chart shows them as gaps instead of interpolating.
// at() addresses the timeline from the oldest point (0) to the newest point
// (POINTS - 1); points without a sample yet return NAN.
//
// No ESPHome or LVGL dependencies: the timeline can be tested on a host PC.
template<size_t POINTS, size_t CHANNELS, uint32_t INTERVAL_MS>
class TrendBuffer {
 public:
  static_assert(POINTS > 0 && CHANNELS > 0 && INTERVAL_MS > 0, "empty trend buffer");

  static constexpr size_t points = POINTS;
  static constexpr size_t channels = CHANNELS;
  using Sample = std::array<float, CHANNELS>;

  void push(const Sample &sample, uint32_t now) {
    if (has_sample_) {
      // Unsigned subtraction also handles the millis() rollover.
      const uint32_t steps = (now - last_sample_ms_) / INTERVAL_MS;
      const size_t gaps = std::min<size_t>(steps > 0 ? steps - 1 : 0, POINTS);
      Sample empty;
      empty.fill(NAN);
      for (size_t i = 0; i < gaps; ++i) append(empty);
    }
    append(sample);
    last_sample_ms_ = now;
    has_sample_ = true;
  }

  float at(size_t point, size_t channel) const {
    if (point >= POINTS || channel >= CHANNELS || point < POINTS - count_) return NAN;
    const size_t offset = point - (POINTS - count_);
    return samples_[(write_ + POINTS - count_ + offset) % POINTS][channel];
  }

 private:
  void append(const Sample &sample) {
    samples_[write_] = sample;
    write_ = (write_ + 1) % POINTS;
    count_ = std::min(count_ + 1, POINTS);
  }

  std::array<Sample, POINTS> samples_{};
  size_t write_ = 0;
  size_t count_ = 0;
  uint32_t last_sample_ms_ = 0;
  bool has_sample_ = false;
};

}  // namespace waveshare_ui
