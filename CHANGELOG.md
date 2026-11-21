# Changelog

All notable changes to this project are documented in this file. The project uses [semantic versioning](https://semver.org/).

## Unreleased

## [1.0.1] - 2026-11-21

### Fixed

- `apply_chart_style()` disables scrolling with `lv_obj_set_scrollable()` instead of the deprecated `lv_obj_remove_flag()`, which caused a compiler warning in every project.

## [1.0.0] - 2026-11-21

### Added

- Board package for the Waveshare ESP32-S3-Touch-LCD-7: ESP32-S3 N16R8 platform, octal PSRAM at 80 MHz, onboard I2C bus and CH422G expander.
- Display package: 800x480 RGB display, GT911 touch with wake-only first touch, backlight switch on EXIO2 and LVGL with a single idle timeout.
- Local backup-battery voltage and voltage-based state of charge on GPIO6.
- Roboto 500 fonts with German umlauts in sizes 14, 16, 18, 20, 24, 28, 32 and 48.
- Shared dark theme, named styles and the selected button state.
- Persistent header with title, firmware version, clock, battery reading and a project status slot.
- Navigation geometry derived from the number of buttons.
- LVGL chart support with a shared chart style and a RAM-only trend buffer.
- Optional Home Assistant motion source for display wake and keep-awake.
- `example.yaml`, which builds the base on its own.
- `scripts/use-local-base.ps1` for developing a project against a local working tree of the base.
- Default text colour `0xFFFFFF` for pages, cards and dialog panels, inherited by labels without their own colour.

### Fixed

- `scripts/use-local-base.ps1` no longer fails when the package file lists a single base file.
