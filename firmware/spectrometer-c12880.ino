#include "elapsedMillis.h"

#include "c12880.h"

#define SPEC_TRG         (-1) //not used/connected in my setup
#define SPEC_VIDEO       (A1)
#define SPEC_CLK         (A2)
#define SPEC_ST          (A3)

// Holds data read out from spectrometer
uint16_t data[C12880_NUM_CHANNELS];

// Controller class
C12880_Class spec(SPEC_TRG,SPEC_ST,SPEC_CLK,SPEC_VIDEO);

/******************************************************************************/

void setup(){
  //Serial.begin(115200); // Baud Rate set to 115200
  spec.begin();
}

/******************************************************************************/

void loop(){
  //int num_bytes = sCmd.readSerial();      // fill the buffer
  //if (num_bytes > 0){
  //  sCmd.processCommand();  // process the command
  //}
  delay(10);
}

/*
void SPEC_INTEG_sCmd_config_handler(SerialCommand this_sCmd){
  char *arg = this_sCmd.next();
  float integ_time;
  if (arg == NULL){
    this_sCmd.print(F("### Error: SPEC.INTEG requires 1 argument 'time' (s)\n"));
  }
  else{
    integ_time = atof(arg);
    spec.set_integration_time(integ_time);
  }
}

void SPEC_READ_sCmd_query_handler(SerialCommand this_sCmd){
  spec.read_into(data);
  for (int i = 0; i < C12880_NUM_CHANNELS - 1; i++){
    this_sCmd.print(data[i]);
    this_sCmd.print(',');
  }
  //last value gets special formatting
  this_sCmd.print(data[C12880_NUM_CHANNELS - 1]);
  this_sCmd.print("\n");
}
*/
