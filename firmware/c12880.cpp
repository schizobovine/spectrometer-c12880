/*
  c12880.cpp - Library for interacting with the Hamamatsu C12880
                          microspectrometer
  Created by Craig Wm. Versek, 2017-03-01
  Altered for Adafruit M4 Feather by scaulfield 2019-11-10
  
  Much of the initial implmentation was based on this project:
  https://github.com/groupgets/c12880ma/blob/master/arduino_c12880ma_example/arduino_c12880ma_example.ino
 */
#include <Arduino.h>
#include <elapsedMillis.h>
#include "c12880.h"

#define ADC_BITS (12)
 
////////////////////////////////////////////////////////////////////////////////
// High performance helper functions

//this function produces a delay for *half* clock period (100ns), approaching 5MHz
//for Teensyduino works up to 180MHz clock such as in the Teensy 3.6
static inline void _ultrashort_delay_100ns(){
  //default to something that should always work in Arduino
  delayMicroseconds(1);
}

////////////////////////////////////////////////////////////////////////////////

C12880_Class::C12880_Class(const int TRG_pin,
                             const int ST_pin,
                             const int CLK_pin,
                             const int VIDEO_pin
                             ){
  _TRG_pin = TRG_pin; // this pin never gets used anywhere...
  _ST_pin  = ST_pin;
  _CLK_pin = CLK_pin;
  _VIDEO_pin = VIDEO_pin;
  _clock_delay_micros = 1; // half of a clock period
  _min_integ_micros = 0;   // this is correction which is platform dependent and 
                           // should be measured in `begin`
  set_integration_time(0.010);  // integration time default to 1ms
}

inline void C12880_Class::_pulse_clock(int cycles){
  for(int i = 0; i < cycles; i++){
    digitalWrite(_CLK_pin, HIGH);
    _ultrashort_delay_100ns();
    digitalWrite(_CLK_pin, LOW);
    _ultrashort_delay_100ns();
  }
}

inline void C12880_Class::_pulse_clock_timed(int duration_micros){
  elapsedMicros sinceStart_micros = 0;
  while (sinceStart_micros < duration_micros){
    digitalWrite(_CLK_pin, HIGH);
    _ultrashort_delay_100ns();
    digitalWrite(_CLK_pin, LOW);
    _ultrashort_delay_100ns();
  }
}

void C12880_Class::begin() {
  //DUE and ZERO (SAMD21) have 12-bit capability
  analogReadResolution(ADC_BITS);
  //Set desired pins to OUTPUT
  pinMode(_CLK_pin, OUTPUT);
  pinMode(_ST_pin, OUTPUT);
  
  digitalWrite(_CLK_pin, LOW); // Start with CLK High
  digitalWrite(_ST_pin, LOW);  // Start with ST Low
  
  _measure_min_integ_micros();
}

void C12880_Class::_measure_min_integ_micros() {
  //48 clock cycles are required after ST goes low
  elapsedMicros sinceStart_micros = 0;
  _pulse_clock(48);
  _min_integ_micros = sinceStart_micros;
}

void C12880_Class::set_integration_time(float seconds) {
  _integ_time = max(seconds, 0);
}


void C12880_Class::read_into(uint16_t *buffer) {
  //compute integration time
  int duration_micros = (int) (_integ_time*1e6);
  duration_micros -= _min_integ_micros; //correction based on 48 pulses after ST goes low
  duration_micros = max(duration_micros,0);
  // Start clock cycle and set start pulse to signal start
  digitalWrite(_CLK_pin, HIGH);
  _ultrashort_delay_100ns();
  digitalWrite(_CLK_pin, LOW);
  digitalWrite(_ST_pin, HIGH);
  _ultrashort_delay_100ns();
  //pixel integration starts after three clock pulses
  _pulse_clock(3);
   _timings[0] = micros();
  //Integrate pixels for a while
  //_pulse_clock(_integ_clock_cycles);
  _pulse_clock_timed(duration_micros);
  //Set _ST_pin to low
  digitalWrite(_ST_pin, LOW);
  _timings[1] = micros();
  //Sample for a period of time
  //integration stops at pulse 48 th pulse after ST went low
  _pulse_clock(48);
  _timings[2] = micros();
  //pixel output is ready after last pulse #88 after ST wen low
  _pulse_clock(40);
  _timings[3] = micros();

  //Read from SPEC_VIDEO
  for(int i = 0; i < C12880_NUM_CHANNELS; i++){
    buffer[i] = analogRead(_VIDEO_pin);
    _pulse_clock(1);
  }
  _timings[4] = micros();

}

