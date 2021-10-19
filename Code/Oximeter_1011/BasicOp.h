/*
 * This file includes basic function to control the LED and PD.
 */
#define basicOp 1

#ifndef constant
#include "Constant.h"
#endif

#ifndef signals
#include "Signal.h"
#endif

#ifndef screen
#include "Screen.h"
#endif

#include <math.h>

float mimic_PD = 0;
int j = 0;
int k = 0;

class Op{

  public:

  void signal_generate(int i);
  void signal_output(int i);
  void signal_detect(int i);

  int getSpO2();
  
};

  // put your main code here, to run repeatedly:
void Op::signal_generate(int i){
  switch (i % 4){
    case 0:
    //analogWrite(NIR,(Vout) * analog_MAX / Vmax);
    digitalWrite(NIR, HIGH);
    //mimic_PD = data_nir[j % 500];
    j++;
    //LED[0] = 1;
    break;

    case 1:
    //analogWrite(NIR,0);
    digitalWrite(NIR, LOW);
    //mimic_PD = 0;
    //LED[0] = 0;
    break;

    case 2:
    //analogWrite(RED, (Vout) * analog_MAX / Vmax);
    digitalWrite(RED,HIGH);
    //mimic_PD = data_red[k % 500];
    k++;
    //LED[1] = 10;
    break;

    case 3:
    //analogWrite(RED,0);
    digitalWrite(RED, LOW);
    //mimic_PD = 0;
    //LED[1] = 0;
    break;

    default:
    break;
  }
}

  //detect PD signal//
void Op::signal_detect(int i){
  switch (i % 4){
      case 0:
      detected_NIR = analogRead(PD);
      //detected_NIR = mimic_PD;
      NIR_Signal.signal_analyze(FIRfilter_lowpass(detected_NIR, Coeffs_sample02, NIR_store_lowpass),micros());
      //NIR_Signal.signal_analyze(detected_NIR,micros());
      //NIR_Signal.putdata(detected_NIR); //store the data in the array//
      //if(!(NIR_Signal.maximum == 0 || NIR_Signal.minimum == 0)){
        //Serial.print(DirectFormIISOS(detected_NIR - (NIR_Signal.maximum + NIR_Signal.minimum) / 2, Coeffs_sample01, p_bx_Gain_sample01));
      //}
      
      
      //for(int k = 0; k < 10; k++){
      //Serial.print(Wn_store[k]);
      
      //}
      //Serial.println(Wn_store[10]);
      //Serial.print(NIR_Signal.maximum);
      //Serial.print(' ');
      //Serial.print(NIR_Signal.minimum);
      //Serial.print(' ');
      //Serial.println(NIR_Signal.heartrate);
      

      //detected_NIR = LED[0] + LED[1];
      break;

      case 2:
      detected_RED = analogRead(PD);
      //detected_RED = mimic_PD;
      RED_Signal.signal_analyze(FIRfilter_lowpass(detected_RED, Coeffs_sample02, RED_store_lowpass),micros()); //store the data in the array//
      //RED_Signal.signal_analyze(detected_RED,micros()); //store the data in the array//
      //Serial.println();
      //if (detected_RED > (analog_MAX * 0.9)){
      //  Vout -= 0.1;
    //  }
      //detected_RED = LED[1] + LED[0];
      break;
      
      default:
      break;
    }
}

//Test for outputing//
void Op::signal_output(int i){

  //Serial.print(RED_Signal.maximum);
  //Serial.print(' ');
 // Serial.print(RED_Signal.minimum);
  //Serial.print(' ');
  switch (i % 4){
    case 0:
     Serial.print(NIR_Signal.get_currentdata());
     Serial.print(' ');
     //Serial.print(FIRfilter_lowpass(NIR_Signal.get_currentdata(), Coeffs_sample02, NIR_store_lowpass));
     //Serial.print(FIRfilter(NIR_Signal.get_currentdata(), Coeffs_sample01, NIR_store));
     //Serial.print(counter);
     //Serial.print(' ');
     break;
    case 2:
     Serial.print(RED_Signal.get_currentdata());
     //Serial.print(' ');
     //Serial.print(FIRfilter_lowpass(RED_Signal.get_currentdata() + 500, Coeffs_sample02, RED_store_lowpass));
     //Serial.print(FIRfilter(RED_Signal.get_currentdata(), Coeffs_sample01, RED_store));
     Serial.print('\n');
     break;
    default:
      break;
  }

 }

//Calculate SpO2

const float Hb_RED = 1;
const float Hb_NIR = 1;
const float HbO2_RED = 1;
const float HbO2_NIR = 1;
int Op::getSpO2(){
  if((RED_Signal.peakpoint.p_timestamp - NIR_Signal.peakpoint.p_timestamp > 500000) 
      && (RED_Signal.troughpoint.p_timestamp - NIR_Signal.troughpoint.p_timestamp > 500000))
      {
        return -1;
      }
  //RED for #1, NIR for #2//
  float R = log(RED_Signal.troughpoint.p_value / RED_Signal.peakpoint.p_value) 
          / log(NIR_Signal.troughpoint.p_value / NIR_Signal.peakpoint.p_value);
  //Serial.println(R);

  float SaO2 = ((Hb_RED - Hb_NIR) * R)
             / ((Hb_RED - HbO2_RED) - (Hb_NIR - HbO2_NIR) * R);
  return R;
}

Op op;
