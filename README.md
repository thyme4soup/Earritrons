# Earritrons
Numitron earrings driven by Arduino

## Introduction

### What it does
These earrings are made from IV-9 Numitrons, old russian 8-segment tube displays. The project runs them in (for now) 3 different modes. The first one spins through numbers like an old casino slot machine, settling on a random number when done. After a brief pause displaying the number, the display turns off for a set amount of time before restarting. The second one displays numbers that switch randomly for a period of time, followed by a similar rest period to save power. The third mode lights up one segment at a time, traveling in a figure-eight around the display. This mode doesn't have a sleep period as it is already low power.

### Overview
The final version for this is still a W.I.P., so there aren't any pictures of them in use. However, there is a working prototype so I may upload pictures of it at some point.

The project is driven by a Nano board using 4 AA batteries. The battery life winds up being about 20 hours of use, but I have the math somewhere so I'll update the Readme when I get around to finalizing the project.

I am also going to upload the schematic once finalized, which is a very straightforward design. However as described below, implementation and build is a bit more complicated

## Earritrons.ino

The only code necessary for the project, designed as a Finite State Machine to allow for easy mode changes via an interrupt attached to a push button.

## PulseTest

This was an implementation that used a square wave signal to power the lights, as in the beginning I used two LED driver ICs to power the numitrons. This led to a current draw to high for the batteries and caused voltage drops. By pulsing one numitron and then the other we could cut the draw in half by cutting the brightness in half, but I realized that I had enough pins on the Nano board to power them directly (which was preferrable because it was easier to solder, regardless of whatever strain I now put on the board)

## Build

Building is still in the works for a final version, but I like how far it's come.

The board, mode button, and cable ends for the earrings are all complete and soldered. The earrings themselves are still in the works.

The main issue is that the numitrons have 9 pins, which means I'd have to run 9 wires to each ear. In addition, I figured it would be best to be able to detach the earrings in case one breaks and make the apparatus easier to take off/put on. Firstly, we don't use the decimal point, so we can knock that down to 8 wires. Secondly, it turns out that ethernet cables have 8 internal wires and have convenient plugs and allow for extending the cables if you need to place the power box for the earrings somewhere random, like hooked on a belt or in a bag. So, we can use ethernet cables as the bus for the wires and then hook the numitron up somehow.

The completed board has 2 short female ethernet cable ends into which the main earring cables can be plugged.

The issue is that ethernet cables are pretty clunky and inflexible, so I still need to find flexible, insulated wire to go the last stretch from the cable to the numitron. I found some for the prototype, but not enough for the final version, so I'm looking for more.

Again, I'm hoping to post a schematic once I start finalizing the build.