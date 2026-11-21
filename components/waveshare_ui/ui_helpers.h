#pragma once

#include <cstddef>
#include <initializer_list>
#include "lvgl.h"

namespace waveshare_ui {

// Marks exactly one button of a group as selected (LV_STATE_CHECKED) and
// clears the state on all others. The theme styles the checked state.
// An out-of-range index clears every button.
inline void set_active_button(std::initializer_list<lv_obj_t *> buttons, int active) {
  int index = 0;
  for (lv_obj_t *button : buttons) {
    if (index == active) {
      lv_obj_add_state(button, LV_STATE_CHECKED);
    } else {
      lv_obj_remove_state(button, LV_STATE_CHECKED);
    }
    ++index;
  }
}

#if LV_USE_CHART
// Applies the shared trend-chart look: dark plot area without border or
// padding, palette grid lines, 2 px series lines and no point markers.
inline void apply_chart_style(lv_obj_t *chart) {
  lv_obj_set_scrollable(chart, false);
  lv_obj_set_style_bg_color(chart, lv_color_hex(0x11171C), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(chart, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(chart, 0, LV_PART_MAIN);
  lv_obj_set_style_line_color(chart, lv_color_hex(0x3B4652), LV_PART_MAIN);
  lv_obj_set_style_line_width(chart, 2, LV_PART_ITEMS);
  lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);
}
#endif

}  // namespace waveshare_ui
