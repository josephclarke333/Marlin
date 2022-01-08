/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2021 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../inc/MarlinConfigPre.h"

#if ENABLED(EASYTHREED_UI)

#include "easythreed_ui.h"
#include "pause.h"
#include "../module/temperature.h"
#include "../module/printcounter.h"
#include "../sd/cardreader.h"
#include "../gcode/queue.h"
#include "../module/motion.h"
#include "../module/planner.h"
#include "../MarlinCore.h"

EasythreedUI easythreed_ui;

#define BTN_DEBOUNCE_MS 20
#define BTN_UNPRESS_MS 1000
#define FEEDRETRACT_MS 60000
#define ERROR_MS 5000
#define ANIMATION_MS 500

void EasythreedUI::init() {
  SET_INPUT_PULLUP(BTN_HOME);    
  SET_OUTPUT(BTN_HOME_LED);
  SET_INPUT_PULLUP(BTN_FEED);     
  SET_OUTPUT(BTN_FEED_LED);
  SET_INPUT_PULLUP(BTN_RETRACT); 
  SET_OUTPUT(BTN_RETRACT_LED);
  SET_INPUT_PULLUP(BTN_PRINT);
  SET_OUTPUT(BTN_PRINT_LED);
}

void EasythreedUI::run() {
  blinkPRINT();
  blinkHOME();
  blinkFEED();
  blinkRETRACT();
  feedButton();
  retractButton();
  printButton();
  homeButton();
  startupAnimation();
}

enum LEDInterval : uint16_t {
  LED_OFF     =    0,
  LED_ON      = 4000,
  LED_BLINK_0 = 2500,
  LED_BLINK_1 = 1500,
  LED_BLINK_2 = 1000,
  LED_BLINK_3 =  800,
  LED_BLINK_4 =  500,
  LED_BLINK_5 =  300,
  LED_BLINK_6 =  150,
  LED_BLINK_7 =   50
};

uint16_t blinkPRINT_interval_ms = LED_OFF;   // Status LED on Print Button

void EasythreedUI::blinkPRINT() {
  static millis_t prev_blinkPRINT_interval_ms = 0, blink_start_ms = 0;

  if (blinkPRINT_interval_ms == LED_OFF) { WRITE(BTN_PRINT_LED, HIGH); return; } // OFF
  if (blinkPRINT_interval_ms >= LED_ON)  { WRITE(BTN_PRINT_LED,  LOW); return; } // ON

  const millis_t ms = millis();
  if (prev_blinkPRINT_interval_ms != blinkPRINT_interval_ms) {
    prev_blinkPRINT_interval_ms = blinkPRINT_interval_ms;
    blink_start_ms = ms;
  }
  if (PENDING(ms, blink_start_ms + blinkPRINT_interval_ms))
    WRITE(BTN_PRINT_LED, LOW);
  else if (PENDING(ms, blink_start_ms + 2 * blinkPRINT_interval_ms))
    WRITE(BTN_PRINT_LED, HIGH);
  else
    blink_start_ms = ms;
}

uint16_t blinkHOME_interval_ms = LED_OFF;   // Status LED on Home Button

void EasythreedUI::blinkHOME() {
  static millis_t prev_blinkHOME_interval_ms = 0, blink_start_ms = 0;

  if (blinkHOME_interval_ms == LED_OFF) { WRITE(BTN_HOME_LED, HIGH); return; } // OFF
  if (blinkHOME_interval_ms >= LED_ON)  { WRITE(BTN_HOME_LED,  LOW); return; } // ON

  const millis_t ms = millis();
  if (prev_blinkHOME_interval_ms != blinkHOME_interval_ms) {
    prev_blinkHOME_interval_ms = blinkHOME_interval_ms;
    blink_start_ms = ms;
  }
  if (PENDING(ms, blink_start_ms + blinkHOME_interval_ms))
    WRITE(BTN_HOME_LED, LOW);
  else if (PENDING(ms, blink_start_ms + 2 * blinkHOME_interval_ms))
    WRITE(BTN_HOME_LED, HIGH);
  else
    blink_start_ms = ms;
}

uint16_t blinkFEED_interval_ms = LED_OFF;   // Status LED on Home Button

void EasythreedUI::blinkFEED() {
  static millis_t prev_blinkFEED_interval_ms = 0, blink_start_ms = 0;

  if (blinkFEED_interval_ms == LED_OFF) { WRITE(BTN_FEED_LED, HIGH); return; } // OFF
  if (blinkFEED_interval_ms >= LED_ON)  { WRITE(BTN_FEED_LED,  LOW); return; } // ON

  const millis_t ms = millis();
  if (prev_blinkFEED_interval_ms != blinkFEED_interval_ms) {
    prev_blinkFEED_interval_ms = blinkFEED_interval_ms;
    blink_start_ms = ms;
  }
  if (PENDING(ms, blink_start_ms + blinkFEED_interval_ms))
    WRITE(BTN_FEED_LED, LOW);
  else if (PENDING(ms, blink_start_ms + 2 * blinkFEED_interval_ms))
    WRITE(BTN_FEED_LED, HIGH);
  else
    blink_start_ms = ms;
}

uint16_t blinkRETRACT_interval_ms = LED_OFF;   // Status LED on Home Button

void EasythreedUI::blinkRETRACT() {
  static millis_t prev_blinkRETRACT_interval_ms = 0, blink_start_ms = 0;

  if (blinkRETRACT_interval_ms == LED_OFF) { WRITE(BTN_RETRACT_LED, HIGH); return; } // OFF
  if (blinkRETRACT_interval_ms >= LED_ON)  { WRITE(BTN_RETRACT_LED,  LOW); return; } // ON

  const millis_t ms = millis();
  if (prev_blinkRETRACT_interval_ms != blinkRETRACT_interval_ms) {
    prev_blinkRETRACT_interval_ms = blinkRETRACT_interval_ms;
    blink_start_ms = ms;
  }
  if (PENDING(ms, blink_start_ms + blinkRETRACT_interval_ms))
    WRITE(BTN_RETRACT_LED, LOW);
  else if (PENDING(ms, blink_start_ms + 2 * blinkRETRACT_interval_ms))
    WRITE(BTN_RETRACT_LED, HIGH);
  else
    blink_start_ms = ms;
}

// Do Fancy Startup Animation On the LED's
void EasythreedUI::startupAnimation()
{
  static millis_t animation_time = 0;
  enum AnimationStatus : uint8_t { AS_HOME, AS_RETRACT, AS_FEED, AS_PRINT, AS_RESET, AS_FINISH };
  static uint8_t animationStep = AS_HOME;
  static bool flag = false;

  switch (animationStep) {
    case AS_HOME:
      if (!flag) {
        blinkHOME_interval_ms = LED_ON;
        flag = true;
        animation_time = millis();
      }
      else if (ELAPSED(millis(), animation_time + ANIMATION_MS*8)) {
        blinkHOME_interval_ms = LED_OFF;
        flag = false;
        animationStep++;
      }
      break;

    case AS_RETRACT:
      if (!flag) {
        blinkRETRACT_interval_ms = LED_ON;
        flag = true;
        animation_time = millis();
      }
      else if (ELAPSED(millis(), animation_time + ANIMATION_MS)) {
        blinkRETRACT_interval_ms = LED_OFF;
        flag = false;
        animationStep++;
      }
      break;

    case AS_FEED:
      if (!flag) {
        blinkFEED_interval_ms = LED_ON;
        flag = true;
        animation_time = millis();
      }
      else if (ELAPSED(millis(), animation_time + ANIMATION_MS)) {
        blinkFEED_interval_ms = LED_OFF;
        flag = false;
        animationStep++;
      }
      break;

    case AS_PRINT:
      if (!flag) {
        blinkPRINT_interval_ms = LED_ON;
        flag = true;
        animation_time = millis();
      }
      else if (ELAPSED(millis(), animation_time + ANIMATION_MS)) {
        blinkPRINT_interval_ms = LED_OFF;
        flag = false;
        animationStep++;
      }
      break;

    case AS_RESET:
      if (!flag) {
        flag = true;
        animation_time = millis();
      }
      else {if (ELAPSED(millis(), animation_time + ANIMATION_MS)) {
        blinkPRINT_interval_ms = LED_ON;
        flag = false;
        animationStep++;}
      }
      break;

    case AS_FINISH:
      break;
  }
}

//
// Filament Load/Unload Buttons
//
bool cancelFeed =  false;
bool cancelRetract =  false;

void EasythreedUI::feedButton() {
  static millis_t filament_time = 0;
  enum FilamentStatus : uint8_t { FS_IDLE, FS_PRESS, FS_CHECK, FS_PROCEED };
  static uint8_t filament_status = FS_IDLE;

  if (printingIsActive()) return; // Don't do anything if print is already started

  if (cancelFeed) { // If feed should be cancelled
    filament_status = FS_IDLE;
    blinkFEED_interval_ms = LED_OFF;
    cancelFeed = false;
  }

  switch (filament_status) {

    case FS_IDLE:
      if (ELAPSED(millis(), filament_time + BTN_UNPRESS_MS)) { 
        if (!READ(BTN_FEED)) {                  // If Feed Button Is pressed...
          filament_status++;                                          // ...proceed to next test.
          filament_time = millis();
        }
      }
      break;

    case FS_PRESS:
      if (ELAPSED(millis(), filament_time + BTN_DEBOUNCE_MS)) {     // After a short debounce delay...
        if (!READ(BTN_FEED)) {                // ...if switch still toggled...
          thermalManager.setTargetHotend(PREHEAT_2_TEMP_HOTEND, 0);  // Start heating up
          blinkFEED_interval_ms = LED_BLINK_7;                       // Set the LED to blink fast
          filament_status++;
          filament_time = millis();
          cancelRetract = true;
          SERIAL_ECHOLN("Heating");
        }
        else {
          filament_status = FS_IDLE;                                // Switch not toggled long enough
          SERIAL_ECHOLN("Debound Detected");
        }
      }
      break;

    case FS_CHECK:
      if (ELAPSED(millis(), filament_time + BTN_UNPRESS_MS)) { // Wait For Button To Be Unpressed
        if (!READ(BTN_FEED)) { 
          SERIAL_ECHOLN("Button Pressed Again, Stopping Heating");                   // Switch Pressed
          blinkFEED_interval_ms = LED_OFF;                                 // LED on steady
          filament_status = FS_IDLE;
          thermalManager.disable_all_heaters();
          filament_time = millis();
        }
        else if (thermalManager.hotEnoughToExtrude(0)) {              // Is the hotend hot enough to move material?
          SERIAL_ECHOLN("Hotend At Temperature");  
          filament_status++;                                          // Proceed to feed / retract.
          blinkFEED_interval_ms = LED_BLINK_5;                            // Blink ~3 times per second
        }
      }
      break;

    case FS_PROCEED:
      // Feed or Retract just once. Hard abort all moves and return to idle on swicth release.
      static bool flag = false;
      if (!READ(BTN_FEED)) {                    // Switch Pressed Stop
        SERIAL_ECHOLN("Button Pressed Again, Stopping Extrusion"); 
        flag = false;                                               // Restore flag to false
        filament_status = FS_IDLE;                                  // Go back to idle state
        thermalManager.disable_all_heaters();                       // And disable all the heaters
        blinkFEED_interval_ms = LED_OFF;
        filament_time = millis();
        quickstop_stepper();                                        // Hard-stop all the steppers ... now!
      }
      else if (!flag) {
        flag = true;
        filament_time = millis();
        SERIAL_ECHOLN("Extruding Filament For 30 Seconds");
        queue.inject(F("G91\nG1 E200 F100")); // Extrude 100mm Of Filament
        //queue.inject(!READ(BTN_RETRACT) ? F("G91\nG0 E10 F180\nG0 E-120 F180\nM104 S0") : F("G91\nG0 E100 F120\nM104 S0"));
      }
      else if ((ELAPSED(millis(), filament_time + FEEDRETRACT_MS))) // If FEEDRETRACT_MS has elapsed trigger saftey timout
      {
        SERIAL_ECHOLN("Safety Timeout, Extruder Cooling..."); 
        flag = false;                                               // Restore flag to false
        filament_status = FS_IDLE;                                  // Go back to idle state
        thermalManager.disable_all_heaters();                       // And disable all the heaters
        queue.clear();                                              // Clear queue
        blinkFEED_interval_ms = LED_OFF;
        quickstop_stepper();                                        // Hard-stop all the steppers ... now!
      }
      break;
  }

}

void EasythreedUI::retractButton() {
  static millis_t filament_time = 0;
  enum FilamentStatus : uint8_t { FS_IDLE, FS_PRESS, FS_CHECK, FS_PROCEED };
  static uint8_t filament_status = FS_IDLE;

  if (printingIsActive()) return; // Don't do anything if print is already started

  if (cancelRetract) { // If feed should be cancelled
    filament_status = FS_IDLE;
    blinkRETRACT_interval_ms = LED_OFF;
    cancelRetract = false;
  }

  switch (filament_status) {

    case FS_IDLE:
      if (ELAPSED(millis(), filament_time + BTN_UNPRESS_MS)) { 
        if (!READ(BTN_RETRACT)) {                  // If Feed Button Is pressed...
          filament_status++;                                          // ...proceed to next test.
          filament_time = millis();
        }
      }
      break;

    case FS_PRESS:
      if (ELAPSED(millis(), filament_time + BTN_DEBOUNCE_MS)) {     // After a short debounce delay...
        if (!READ(BTN_RETRACT)) {                // ...if switch still toggled...
          thermalManager.setTargetHotend(PREHEAT_2_TEMP_HOTEND, 0);  // Start heating up
          blinkRETRACT_interval_ms = LED_BLINK_7;                       // Set the LED to blink fast
          filament_status++;
          filament_time = millis();
          cancelFeed = true;
          SERIAL_ECHOLN("Heating");
        }
        else {
          filament_status = FS_IDLE;                                // Switch not toggled long enough
          SERIAL_ECHOLN("Debound Detected");
        }
      }
      break;

    case FS_CHECK:
      if (ELAPSED(millis(), filament_time + BTN_UNPRESS_MS)) { // Wait For Button To Be Unpressed
        if (!READ(BTN_RETRACT)) { 
          SERIAL_ECHOLN("Button Pressed Again, Stopping Heating");                   // Switch Pressed
          blinkRETRACT_interval_ms = LED_OFF;                                 // LED on steady
          filament_status = FS_IDLE;
          thermalManager.disable_all_heaters();
          filament_time = millis();
        }
        else if (thermalManager.hotEnoughToExtrude(0)) {              // Is the hotend hot enough to move material?
          SERIAL_ECHOLN("Hotend At Temperature");  
          filament_status++;                                          // Proceed to feed / retract.
          blinkRETRACT_interval_ms = LED_BLINK_5;                            // Blink ~3 times per second
        }
      }
      break;

    case FS_PROCEED:
      // Feed or Retract just once. Hard abort all moves and return to idle on swicth release.
      static bool flag = false;
      if (!READ(BTN_RETRACT)) {                    // Switch Pressed Stop
        SERIAL_ECHOLN("Button Pressed Again, Stopping Retractuon"); 
        flag = false;                                               // Restore flag to false
        filament_status = FS_IDLE;                                  // Go back to idle state
        thermalManager.disable_all_heaters();                       // And disable all the heaters
        blinkRETRACT_interval_ms = LED_OFF;
        filament_time = millis();
        quickstop_stepper();                                        // Hard-stop all the steppers ... now! Seems to be broken?
      }
      else if (!flag) {
        flag = true;
        filament_time = millis();
        SERIAL_ECHOLN("Retracting Filament For 30 Seconds");
        queue.inject(F("G91\nG1 E10 F100\nG1 E-200 F100")); // Extrude 100mm Of Filament
        //queue.inject(!READ(BTN_RETRACT) ? F("G91\nG0 E10 F180\nG0 E-120 F180\nM104 S0") : F("G91\nG0 E100 F120\nM104 S0"));
      }
      else if ((ELAPSED(millis(), filament_time + FEEDRETRACT_MS))) // If FEEDRETRACT_MS has elapsed trigger saftey timout
      {
        SERIAL_ECHOLN("Safety Timeout, Extruder Cooling..."); 
        flag = false;                                               // Restore flag to false
        filament_status = FS_IDLE;                                  // Go back to idle state
        thermalManager.disable_all_heaters();                       // And disable all the heaters
        queue.clear();                                              // Clear queue
        blinkRETRACT_interval_ms = LED_OFF;
        quickstop_stepper();                                        // Hard-stop all the steppers ... now!
      }
      break;
  }

}

#if HAS_STEPPER_RESET
  void disableStepperDrivers();
#endif

//
// Print Start/Pause/Resume Button
//
void EasythreedUI::printButton() {
  enum KeyStatus : uint8_t { KS_IDLE, KS_PRESS, KS_PROCEED };
  static uint8_t key_status = KS_IDLE;
  static millis_t key_time = 0;
  static millis_t error_time = 0;

  enum PrintFlag : uint8_t { PF_START, PF_PAUSE, PF_RESUME };
  static PrintFlag print_key_flag = PF_START;

  const millis_t ms = millis();

  if ((ELAPSED(millis(), error_time + ERROR_MS)) && blinkPRINT_interval_ms == LED_BLINK_7) {
    blinkPRINT_interval_ms = LED_ON;
  }

  switch (key_status) {
    case KS_IDLE:
      if (!READ(BTN_PRINT)) {                                       // Print/Pause/Resume button pressed?
        key_time = ms;                                              // Save start time
        key_status++;                                               // Go to debounce test
      }
      break;

    case KS_PRESS:
      if (ELAPSED(ms, key_time + BTN_DEBOUNCE_MS))                  // Wait for debounce interval to expire
        key_status = READ(BTN_PRINT) ? KS_IDLE : KS_PROCEED;        // Proceed if still pressed
      break;

    case KS_PROCEED:
      if (!READ(BTN_PRINT)) break;                                  // Wait for the button to be released
      key_status = KS_IDLE;                                         // Ready for the next press
      if (PENDING(ms, key_time + 1200 - BTN_DEBOUNCE_MS)) {         // Register a press < 1.2 seconds
        switch (print_key_flag) {
          case PF_START: {                                          // The "Print" button starts an SD card print
            if (printingIsActive()) break;                          // Already printing? (find another line that checks for 'is planner doing anything else right now?')
            blinkPRINT_interval_ms = LED_BLINK_2;                        // Blink the indicator LED at 1 second intervals
            print_key_flag = PF_PAUSE;                              // The "Print" button now pauses the print
            card.mount();                                           // Force SD card to mount - now!
            if (!card.isMounted) {                                  // Failed to mount?
                blinkPRINT_interval_ms = LED_BLINK_7;                        // Blink LED Fast to indicate card mount error
                SERIAL_ECHOLN("Error Mounting SD Card!"); 
                print_key_flag = PF_START;
                key_status = KS_IDLE;
                error_time = millis();
                return;                                             // Bail out
            }
            card.ls();                                            // List all files to serial output
            const uint16_t filecnt = card.countFilesInWorkDir();  // Count printable files in cwd
            if (filecnt == 0) { // None are printable?
              blinkPRINT_interval_ms = LED_BLINK_7; 
              print_key_flag = PF_START;
              key_status = KS_IDLE;
              error_time = millis();
              SERIAL_ECHOLN("No Printable Files Found!"); 
              return;
            }
            card.selectFileByIndex(filecnt);                      // Select the last file according to current sort options
            card.openAndPrintFile(card.filename);                 // Start printing it
            SERIAL_ECHOLN("Printing....");
            SERIAL_ECHOLN(card.filename); 
            break;
          }
          case PF_PAUSE: {                                          // Pause printing (not currently firing)
            if (!printingIsActive()) break;
            blinkPRINT_interval_ms = LED_ON;                             // Set indicator to steady ON
            SERIAL_ECHOLN("Pausing Print..."); 
            queue.inject(F("M25"));                                 // Queue Pause
            print_key_flag = PF_RESUME;                             // The "Print" button now resumes the print
            break;
            }
          case PF_RESUME: {                                         // Resume printing
            if (printingIsActive()) break;
            blinkPRINT_interval_ms = LED_BLINK_2;                        // Blink the indicator LED at 1 second intervals
            SERIAL_ECHOLN("Resuming Print..."); 
            queue.inject(F("M24"));                                 // Queue resume
            print_key_flag = PF_PAUSE;                              // The "Print" button now pauses the print
            break;
          }
        }
      }
      else {                                                        // Register a longer press
        if (print_key_flag == PF_START && !printingIsActive())  {   // While not printing, this moves Z up 10mm
          blinkPRINT_interval_ms = LED_ON;
          SERIAL_ECHOLN("Moving Z Up 10mm");
          queue.inject(F("G91\nG0 Z10 F600\nG90"));                 // Raise Z soon after returning to main loop
        }
        else {           
          SERIAL_ECHOLN("Cancelling Print...");                                           // While printing, cancel print
          card.abortFilePrintSoon();                                // There is a delay while the current steps play out
          blinkPRINT_interval_ms = LED_ON;                              // Turn off LED
        }
        planner.synchronize();                                      // Wait for commands already in the planner to finish
        TERN_(HAS_STEPPER_RESET, disableStepperDrivers());          // Disable all steppers - now!
        SERIAL_ECHOLN("Print Cancelled");
        print_key_flag = PF_START;                                  // The "Print" button now starts a new print
      }
      break;
  }
}

void EasythreedUI::homeButton() {
  static millis_t home_time = 0;
  enum HomeStatus : uint8_t { HS_IDLE, HS_PRESS, HS_PROCEED };
  static uint8_t home_status = HS_IDLE;

  if (printingIsActive()) return; // Don't do anything if print is already started

  switch (home_status) {

    case HS_IDLE:
      if (ELAPSED(millis(), home_time + BTN_UNPRESS_MS)) { 
        if (!READ(BTN_HOME)) {                  // If Home Button Is pressed...
          home_status++;                                          // ...proceed to next test.
          home_time = millis();
        }
      }
      break;

    case HS_PRESS:
      if (ELAPSED(millis(), home_time + BTN_DEBOUNCE_MS)) {     // After a short debounce delay...
        if (!READ(BTN_HOME)) {                // ...if switch still toggled...
          blinkHOME_interval_ms = LED_BLINK_4;                       // Set the LED to blink
          home_status++;
          SERIAL_ECHOLN("Homing");
        }
        else {
          home_status = HS_IDLE;                                // Switch not toggled long enough
          SERIAL_ECHOLN("Debound Detected");
        }
      }
      break;

    case HS_PROCEED:
      static bool flag = false;

      if (!flag) {
        queue.inject(F("G28")); // Queue Home
        flag = true;
      } 
      else if (all_axes_homed()) {
        home_status = HS_IDLE;
        blinkHOME_interval_ms = LED_OFF;
        quickstop_stepper();                                        // Hard-stop all the steppers ... now!
      }
      break;
  }

}

#endif // EASYTHREED_UI