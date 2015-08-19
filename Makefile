ARDUINO_DIR = /Users/stepcut/projects/arduino/Arduino.app/Contents/Resources/Java/

TARGET = cruiserlights
ARDUINO_LIBS = SPI TCL

BOARD_TAG    = uno
ARDUINO_PORT = /dev/tty.usbmodem1411

include ../Arduino-Makefile/Arduino.mk
