# Waveshare ESPHome Base

Shared ESPHome base package for HMIs on the Waveshare [ESP32-S3-Touch-LCD-7](https://docs.waveshare.com/ESP32-S3-Touch-LCD-7) (800×480 RGB, GT911 touch, CH422G expander, ESP32-S3 N16R8). It gives every project the same hardware setup, look and UI frame, so a project only adds its pages, data sources and commands.

## Contents

| File | Provides |
|---|---|
| `base.yaml` | entry point that composes all files below |
| `board.yaml` | ESP32-S3 platform, 16 MB QIO flash, octal PSRAM at 80 MHz, onboard I2C bus, CH422G |
| `display.yaml` | RGB display, GT911 touch, backlight switch on EXIO2, LVGL with idle timeout and wake-only first touch |
| `battery.yaml` | local backup-battery voltage (GPIO6) and voltage-based state of charge |
| `fonts.yaml` | Roboto 500 with German umlauts in sizes 14, 16, 18, 20, 24, 28, 32 and 48 |
| `theme.yaml` | dark palette, button states and named styles |
| `header.yaml` | persistent 800×58 header with title, version, clock, battery reading and status slot |
| `navigation.yaml` | geometry for equally sized bottom navigation buttons |
| `charts.yaml` | native LVGL chart support |
| `optional/ha-motion.yaml` | optional Home Assistant motion source for display wake and keep-awake |
| `components/waveshare_ui/` | C++ helpers for lambdas |
| `example.yaml` | stand-alone configuration that builds the base |

Not included, by design: network services (`wifi`, `api`, `ota`, `web_server`, `time`), secrets, diagnostics sensors, pages and command backends. They belong to the project.

## Hardware Requirements

- Waveshare ESP32-S3-Touch-LCD-7, the touch model (not 7B or 7C).
- For the battery reading: a 1S backup battery on J3 and a jumper from TP1 to J8 pin 3 / AD (GPIO6). Without the jumper the reading is unreliable. Never connect the battery directly to the ADC input.

## Usage

The project references the base in one committed package file, for example `packages/waveshare-base.yaml`:

```yaml
# Loads the shared Waveshare base at the tag pinned by waveshare_base_ref.
packages:
  waveshare_base:
    url: https://github.com/anwa/waveshare-esphome-base
    ref: ${waveshare_base_ref}
    files: [base.yaml]

external_components:
  - source: github://anwa/waveshare-esphome-base@${waveshare_base_ref}
    components: [waveshare_ui]
```

Keep the `files:` list on one line; `scripts/use-local-base.ps1` reads it. Add `optional/ha-motion.yaml` to the list to enable the motion source.

The root configuration pins the tag and includes the package file through a substitution, which allows switching to a local working tree during development:

```yaml
substitutions:
  waveshare_base_ref: v1.0.0
  waveshare_base_package: packages/waveshare-base.yaml

  firmware_version: "1.2.3"
  ui_header_title: "My HMI"
  battery_entity_prefix: "HMI"
  ui_nav_count: "5"

packages:
  waveshare_base: !include ${waveshare_base_package}
```

The project also defines its network services, with `sntp_time` as time source and `native_api` as API ID when the motion package is used. `example.yaml` shows a complete configuration.

## Substitutions

| Name | Required | Default | Purpose |
|---|---|---|---|
| `firmware_version` | yes | – | project version shown in the header |
| `ui_header_title` | yes | – | product name in the header |
| `battery_entity_prefix` | yes | – | prefix of the battery entity names, for example `HMI` → `HMI Battery Voltage` |
| `ui_nav_count` | yes | – | number of bottom navigation buttons |
| `display_motion_entity` | with motion package | – | Home Assistant entity ID of the motion source |
| `display_idle_timeout` | no | `60s` | inactivity until the backlight turns off |
| `ui_default_font` | no | `font_14` | LVGL default font |
| `ui_local_battery_label` | no | `HMI` | prefix of the battery reading in the header |
| `ui_header_status_width` | no | `100` | width of the header status slot in pixels |
| `pin_i2c_sda`, `pin_i2c_scl`, `pin_touch_irq`, `pin_battery_adc` | no | `GPIO8`, `GPIO9`, `GPIO4`, `GPIO6` | board pins |

`navigation.yaml` provides `ui_nav_y`, `ui_nav_height`, `ui_nav_width`, `ui_nav_x0` and `ui_nav_step`. Button `i` (0-based) is placed at `x: ${ ui_nav_x0 + i * ui_nav_step }`, `y: ${ui_nav_y}`. Six buttons are 120 px wide, five buttons 146 px.

`waveshare_base_version` holds the version of the base.

## Provided IDs

| Kind | IDs |
|---|---|
| hardware | `board_i2c`, `io_expander`, `ws_display`, `ws_touchscreen`, `display_backlight` |
| display activity | `display_register_activity` (script, parameter `wake: bool`), `display_motion_active` |
| battery | `local_backup_battery_voltage`, `local_backup_battery_soc` |
| header | `ui_header`, `ui_header_clock`, `ui_header_battery`, `ui_header_status` |
| fonts | `font_14`, `font_16`, `font_18`, `font_20`, `font_24`, `font_28`, `font_32`, `font_48` |
| styles | `style_page`, `style_header`, `style_card`, `style_button`, `style_nav_button`, `style_dialog_overlay`, `style_dialog_panel`, `style_dialog_button`, `style_dialog_step_button`, `style_dialog_spinbox`, `style_title_label`, `style_desc_label`, `style_value_label` |

The project sets text and colour of `ui_header_status`. Call `display_register_activity` with `wake: true` from any other local activity source that should wake the display.

## C++ Helpers

| Helper | Purpose |
|---|---|
| `waveshare_ui::set_active_button({id(a), id(b), …}, index)` | marks one button as selected (checked state) and clears the others |
| `waveshare_ui::apply_chart_style(chart)` | applies the shared look to an `lv_chart` |
| `waveshare_ui::TrendBuffer<POINTS, CHANNELS, INTERVAL_MS>` | RAM-only trend history; missed intervals become gaps (NAN) |

## Look and Behavior

- Palette: page `0x101418`, header and cards `0x171D23`, buttons and dialog panels `0x202830`, borders `0x303A44` / `0x4A5662`, labels `0x8B96A0`, values `0xFFFFFF`, disabled `0x161B20` / `0x68727B`, selected `0x36505A` / `0x5CFF7A`.
- Card radius 12, button radius 8, dialog panel radius 10. Dialogs are full-screen overlays (`style_dialog_overlay`) with a centred `style_dialog_panel`.
- After `display_idle_timeout` without touch or motion, LVGL pauses and the backlight turns off. The next touch only wakes the display; the control underneath is not operated.
- The header battery reading is red below 3.50 V, yellow from 3.50 V to below 3.70 V and green from 3.70 V. The thresholds are hints, not a protection.

## Developing a Project Against a Local Base

Clone this repository next to the project, then run in the project root:

```powershell
..\waveshare-esphome-base\scripts\use-local-base.ps1
```

The script writes `packages/waveshare-base.local.yaml`, which loads the same files and the `waveshare_ui` component from the local working tree. The project must ignore `*.local.yaml`; the script refuses to write the file otherwise. Build against the local base with:

```powershell
esphome -s waveshare_base_package packages/waveshare-base.local.yaml compile <root yaml>
```

Without `-s` the project builds against the pinned tag again. Before committing the project, set `waveshare_base_ref` to a published release tag and validate without `-s`.

For a hardware test through the Home Assistant ESPHome add-on, which only sees GitHub, publish a pre-release tag such as `v1.1.0-rc.1` and point `waveshare_base_ref` at it.

## Validation

```powershell
.\scripts\validate.ps1 -Setup
```

Runs `esphome config` and `esphome compile` for `example.yaml` with the ESPHome version pinned in `requirements-dev.txt`. `tests/trend_buffer_test.cpp` is a host test for `TrendBuffer`; build it with any C++17 compiler as described in the file.

## License

MIT, see `LICENSE`.
