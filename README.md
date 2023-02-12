## ATMEGA328p composite NTSC video generation

### What is this?

This is the implementation of a very crude and limited frame buffer in less than 2KB of RAM. There are functions to print text and graphics in a 144x100 or 128x100 pixel monochrome resolution. Both sync and video signals are generated, and the MCU can barely keep up with the processing.

### How to wire this up?

You will need an ATMEGA328p running at 16MHz. It can be an arduino nano or a prototype board with an MCU, capacitors and a crystal. Video and sync signals are generated on pins PD5 and PD6 respectively. Wire a 470 ohm resistor to the video line, a 1k ohm resistor to the sync line and tie both ends - this is the video signal output. To protect the MCU pins, wire a diode to each pin (a 1N4148 will do, anode facing the MCU) in series with the resistors.

