# Game Boy Emulator

Game Boy emulator written in C++ with an SDL3-based frontend.

## Build

Requirements:

- Windows
- Visual Studio 2022

1. Open `GameBoyEmulator.sln` in Visual Studio.
2. Select `Release` and `x64` (or `x86`).
3. Build and run the project.

SDL3 is included in `libs/SDL3`.

## Usage

Drag and drop a `.gb` ROM file into the emulator window.

Controls:

- `Z`: A
- `X`: B
- `Enter`: Start
- `Backspace`: Select
- Arrow keys: D-pad

## Current features

- Full Game Boy CPU instruction set, including interrupts.
- System bus with memory mapping, timer and joypad input.
- PPU with background, window, sprites and OAM DMA.
- APU with the four sound channels.
- Cartridge loading with battery-backed saves.
- Cartridge mappers:
  - ROM only
  - MBC1
  - MBC3

## Future work

- Add support for more cartridge types.
- Implement the `STOP` instruction.
- Extend the emulator to support Game Boy Color.

## License

The original code is licensed under the MIT License. See [LICENSE](LICENSE).

SDL3 is distributed under its own license. See [libs/SDL3/LICENSE.txt](libs/SDL3/LICENSE.txt).