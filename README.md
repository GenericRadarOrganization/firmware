# Firmware
Firmware for the teensy-lc microcontroller board.

# Toolchain Requirements
* arm-none-eabi gcc toolchain
* teensy_loader_cli

# To build:
1. Clean repo: `make clean`
2. Build the .hex file: `make`
3. Burn to the teensy-lc board `make burn`

# Software Requirements
1. VCO Control Waveform Output:
   * Must be able to output a voltage waveform using DAC
   * Must have a configurable waveform shape (Saw, triangle, sine, etc.)
   * TBD
2. Down-Mixed Waveform Input:
   * TBD
3. User Interface:
   * Must allow configuration of voltage waveform
   * Must allow configuration of rx'd voltage processing
