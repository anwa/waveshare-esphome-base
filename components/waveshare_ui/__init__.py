"""Shared C++ helpers for Waveshare ESP32-S3-Touch-LCD-7 HMIs.

The component has no configuration and generates no code. Loading it makes
ESPHome copy its headers into the build and include them in every generated
translation unit, so YAML lambdas can call the helpers directly. Headers from
`esphome.includes` cannot be used for this, because ESPHome resolves those
paths relative to the root configuration, not to a remote package.
"""

import esphome.config_validation as cv

CODEOWNERS = ["@anwa"]
DEPENDENCIES = ["lvgl"]

CONFIG_SCHEMA = cv.Schema({})


async def to_code(config):
    pass
