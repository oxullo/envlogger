# ENVLOGGER v1 Hardware tester

Tester firmware for envlogger v1

## Compilation and flashing

Using Particle desktop IDE (https://docs.particle.io/tutorials/developer-tools/dev/).
Ensure that the local compiler toolchain is installed.

1. CTRL-SHIFT-p (or CMD-SHIFT-p on OSX) -> Particle: Install library -> Adafruit_BME680
2. CTRL-SHIFT-p (or CMD-SHIFT-p on OSX) -> Particle: Flash application (local)

Note: the photon will detach from the cloud since the firmware configures the unit to run
in MANUAL system mode.
