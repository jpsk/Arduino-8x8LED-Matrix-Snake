# Arduino-8x8LED-Matrix-Snake
Fun game project for Arduino mega board i did in my spare time

#### Overview
This project uses **Arduino Mega 2560** board, **8X8 LED Matrix** that is controlled by **MAX7219ENG**. The communication between Arduino and MAX chip is via **I<sup>2</sup>C** protocol.

#### Features
- Fully working *Snake* game on 8x8 matrix;
- Serial and hardware controls;

#### Setup
 - 8x8 LED Matrix is connected to Arduino mega like this: **12, 11, 10** for **DIN, CLK, CS**
- For physical controlls I am using mechanical switches with pull-down resistors. You can also control a game by sending commands through an arduino's serial monitor ('u', 'r', 'l', 'd' -> up, right, left, down).
- Just plug, upload and play :)
