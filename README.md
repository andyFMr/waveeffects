# Wave Effects Demo (PVSnesLib)

This is an SNES homebrew demo created using [PVSnesLib](https://github.com/alekmaul/pvsneslib). It demonstrates a per-scanline wave distortion effect using HDMA (Horizontal Direct Memory Access). 

The effect is applied to BG0 (Mode 1), while a text overlay is displayed on BG2 to show the current wave parameters.

## Features

- **HDMA Wave Effect:** Smooth, hardware-accelerated per-scanline wave distortion.
- **Adjustable Parameters:** Real-time control over wave amplitude and frequency.
- **Direction Toggle:** Switch between horizontal and vertical wave distortion.

## Controls

You can use a standard SNES controller to manipulate the wave effect in real-time:

- **Up / Down:** Increase or decrease wave amplitude (+ / -)
- **Left / Right:** Increase or decrease wave frequency (+ / -)
- **L / R (Bumpers):** Toggle wave direction (Horizontal / Vertical)
- **Start:** Reset all parameters to their default values

## Building

To build the project, you must have the `PVSNESLIB_HOME` environment variable set up correctly, pointing to your PVSnesLib installation.

For installation instructions, please refer to the [PVSnesLib Wiki](https://github.com/alekmaul/pvsneslib/wiki/Installation).

Once your environment is set up, you can compile the ROM by running:
```bash
make
```

This will produce a `WaveFx.sfc` ROM file in the project directory.

## Project Structure

- `src/main.c`: Contains the main game loop, initialization, and input handling.
- `src/wave.c`: Handles the core logic for the HDMA wave effect and parameter updates.
- `Makefile`: Build configuration for compiling the ROM.
