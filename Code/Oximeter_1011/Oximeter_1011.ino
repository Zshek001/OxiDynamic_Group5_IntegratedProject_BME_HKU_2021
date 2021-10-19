
#ifndef filter
#include "Filter.h"
#endif

#ifndef screen
#include "Screen.h"
#endif

#ifndef signals
#include "Signal.h"
#endif

#ifndef point
#include "Point.h"
#endif

#ifndef constant
#include "Constant.h"
#endif

#ifndef basicOp
#include "BasicOp.h"
#endif





void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Setup the pinmode//
  pinMode(PD,INPUT);
  pinMode(RED,OUTPUT);
  pinMode(NIR,OUTPUT);

  // Set the resolution(0 - 4095)//
  analogReadResolution(12);
  analogWriteResolution(12);

  //initiallize//
  RED_Signal.cleardata();
  NIR_Signal.cleardata();
  OLEDinit();
  OLEDtitle();
}

void loop() {

  // put your main code here, to run repeatedly:
  loop_start = micros();  //Record the start time of the loop//
  op.signal_generate(i);
  delayMicroseconds(loop_start + PDdelay - micros());
  op.signal_detect(i);

  op.signal_output(i);
  if(i % 200 == 0){
      OLEDheartrate(NIR_Signal.heartrate);
      OLEDR(op.getSpO2());
      OLED.display();
  }
  
  i++;

  /*if(i >= 4000){
    while(1);
  }*/
  loop_end = micros();
  if(loop_start + loop_period - loop_end > 0) delayMicroseconds(loop_start + loop_period - loop_end);
  //else Serial.println("Error");

}
