ARDUINO_DIR = /usr/share/arduino

TARGET = thecloak
ARDUINO_LIBS = SPI TCL

BOARD_TAG    = uno
ARDUINO_PORT = /dev/ttyACM0

include /home/stepcut/n-heptane/projects/arduino/Arduino-Makefile/Arduino.mk
