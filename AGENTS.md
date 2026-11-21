# Agent Instructions

## Inherited Rules

This repository lives in a rule hierarchy. Apply these files in addition to the rules below, in this order of increasing precedence:

| Level | File |
|---|---|
| Global | `../../../AGENTS.md` |
| ESPHome | `../../AGENTS.md` and `../../rules/yaml-comments.md` |
| Waveshare | `../AGENTS.md` |

They define language, Git authority, branches, commits, versioning principles, documentation style, the ESPHome toolchain and validation, YAML comments and the Waveshare board, LVGL and UI conventions. This file contains only what is specific to this repository and takes precedence over the inherited files.

If the inherited files are not present, for example in a fresh clone: write all repository content in English, never commit secrets, and never push, tag or publish without the user's approval.

## Purpose

This repository is the shared ESPHome base for every HMI on the Waveshare ESP32-S3-Touch-LCD-7. It owns the board, display, touch, backlight, LVGL setup, local backup battery, fonts, theme, persistent header, navigation geometry and chart support. Pages, Home Assistant imports, command backends, network services, diagnostics and CAN stay in the consuming projects.

Consuming projects load the base from GitHub at a release tag, see `README.md`.

## Versioning

- The single version source is the `waveshare_base_version` substitution in `base.yaml`.
- Release tags are annotated `vMAJOR.MINOR.PATCH`; test builds for hardware tests use pre-release tags `vMAJOR.MINOR.PATCH-rc.N`. A pushed tag is never moved or deleted, because the Home Assistant ESPHome add-on caches remote packages by ref.
- MAJOR: an incompatible contract change, such as a renamed or removed ID, style, font or substitution, or a new required substitution. Propose it to the user first.
- MINOR: a visible change to look or behavior, or new capability.
- PATCH: fixes without visible change.
- Maintain `CHANGELOG.md` under `Unreleased` in the same change; a release moves the entries into a dated version section.

## Contract

- The IDs, styles, fonts, substitutions and C++ helpers listed in `README.md` are the public contract. Change them only with the matching version increase and update `README.md` in the same change.
- Entity names are derived from `battery_entity_prefix`. Never change the name pattern of an entity, because consuming projects rely on stable Home Assistant entity IDs.
- No `!secret` and no network services in this repository: secrets and network configuration belong to the consuming project.
- C++ helpers live in `components/waveshare_ui/` and are loaded as an external component. Do not use `esphome.includes`; ESPHome resolves those paths relative to the root configuration, which breaks inside a remote package.
- Every visible change to look or behavior must be named to the user before it is implemented.

## Validation

- `scripts/validate.ps1` runs `esphome config` and `esphome compile` for `example.yaml`.
- `tests/trend_buffer_test.cpp` is a host test for `TrendBuffer` and needs a host C++17 compiler.
- Before a release, also validate every consuming project against the release candidate with `scripts/use-local-base.ps1` or the pre-release tag.
