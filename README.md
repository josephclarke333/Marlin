# Marlin 3D Printer Firmware

![GitHub](https://img.shields.io/github/license/marlinfirmware/marlin.svg)
![GitHub contributors](https://img.shields.io/github/contributors/marlinfirmware/marlin.svg)
![GitHub Release Date](https://img.shields.io/github/release-date/marlinfirmware/marlin.svg)
[![Build Status](https://github.com/MarlinFirmware/Marlin/workflows/CI/badge.svg?branch=bugfix-2.0.x)](https://github.com/MarlinFirmware/Marlin/actions)

## Introduction
Fork of marlin configured and compiled for the easythreed X1. My aim is to provide the most up to date version of marlin configured to work best for the X1 (and more easythreed printers if I find someone able to test the firmware me).

## Features
- Backlash Compensation - By default X 0.4mm, Y 0.5mm, Z 0mm
- Bed Skew Compensation - Needs to be manually calibrated using M852 as it will vary significantly for each printer
- Handling Of Physical Buttons - I did my best to re-write the libraries that handle the UI on the easythreed K7 to work for the X1 (given the K7 only had one button, one led and a switch I needed to re-write  most of it). Following is a simple overview of how I've tried to implement the buttons
  - Each button flashes in succession apon powering on the printer (or resetting/connecting to USB) to indicate the printer is ready
  - The home button flashes after being pressed while homing all axis' before turning back off again and disabling steppers, much like the stock firmware
  - The retract button does pretty much the same thing as the stock firmware as well flashing fast indicate heating and slower as it begins to retract filament. It also extruder 10mm first to try to avoid a "bulge" in the end of the  filament which gets stuck in the extruder gears
  - The feed button does pretty much  the same as stock
  - Print button currently operates very much like the stock firmware with one press printing the first file on the SD card and a single press while printing causes a pause/resume. It also flashes fast if there is some sort of error mounting the sd or reading the time.
  - There are defines at the start of easythreed_ui.cpp to set the button debounce, unpress, animation speed, ect.
  
## To do
- Add the ability to select from the first 4 files on the SD using the 4 LEDs in each button to signify which button file is being selected.
- Create an entry in boards.h and a pins_easythreedX1.h file to define pins used for LEDs and Buttons instead of using pins_SANGUINOLOLU_11.h
- Better calibrate default backlash compensation
- Add a version for the old X264 motor version (or you could simply change the default max feedrates and compile the firmware yourself)
- Update the config for other prints like the X2, X3, X4, K1, K2, K3, K4, K7, ect. If you want firmware configured for one of these printers let me know as it shouldn't be too hard to change it over to work with any of those printers.
- Add some sort of tutorial on compiling and/or installing the firmware

## Compiling/Installing
This will just be quick and simple for now but I'll update it later to make it easier to follow

Installing:
- Click Code>Download Zip>Save to Wherever You Want
- Unzip folder
- Open the folder where you saved it and open  Marlin>.pio>build>sanguino1284p_optimized and copy firmware.hex from that folder to somehwere you will remember
- Connect your printer to your computer via USB
- Open cura or prusa slicer
  - In cura - Click Settings>Your_Printer_name>Manage Printers Then in the window that popped up select your printer and click Update Firmware>Upload Custom Firmware and slect the hex file
  - In prusa slicer - Click  Configuration>Flash Printer Firmware Select the firmware.hex file you copied before and the serial port your printer is connected to and  click  Flash!
  - If flashing fails (which it often seems to on the X1 you will need to compile from source and upload with platformio and autobuildmarlin

Compiling:
- Click Code>Download Zip>Save to Wherever You Want
- Unzip folder
- Connect your printer to your computer via USB
- Setup visual studio code with platformIO and Auto Build Marlin extensions
- Click the Auto Build Marlin Menu from the bar on the left
- Click Open Marlin 2.X folder and select the Marlin folder that you extracted from the zip
- Click Show ABM Panel on the left bar
- Click the Auto Build Marlin Panel that juts came up
- Select upload from next to the Environments>sanguino128p_optimized section and wait for it to compile and upload

Post Installation Steps:
- Run the following to set up the EEPROM with new config:
  - M502
  - M500
  - M501
- (Optional)- Calibrate steps per mm for extruder

## Notes
- The new firmware  uses a baudrate of 250000, so if you have trouble connecting over usb try changing your baudrate.

## Disclaimer
I am NOT a programmer not am I a 3D printer expert just an enthusiast who likes to work with code and 3D printers.
**You are downloading, compiling, installing and using this firmware at your own risk and I take no responsibility to any damage caused to you, your printer or any other person/item as a result of this firmwarea**
This firmware is currently only tested by me on my two easytrheed X1's so isn't necesarrily stable (do not I also print almost exclusively using octoprint so not  much testing on the physical buttons has been done)
Any feedback is apreciated and HAVE FUN :)
