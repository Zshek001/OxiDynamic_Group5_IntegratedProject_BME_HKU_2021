/*
   This file includes basic function to control the LED and PD.
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


class Op {

  public:
    int HR;
    float SpO2;

    void signal_generate(int i);
    void signal_output(int i);
    void signal_detect(int i);
    void MotorOn(int i);
    void MotorOff();

    void getHRSpO2();

};

void Op::MotorOn(int i) {
  switch (i % 400) {
    case 0:
      analogWrite(Motor, analog_MAX);
      break;
    case 200:
      analogWrite(Motor, 0);
      break;
    default:
      break;
  }
}

void Op::MotorOff() {
  analogWrite(Motor, 0);
}

// put your main code here, to run repeatedly:
void Op::signal_generate(int i) {
  switch (i % 4) {
    case 0:
      digitalWrite(NIR, HIGH);
      break;

    case 1:
      digitalWrite(NIR, LOW);
      break;

    case 2:
      digitalWrite(RED, HIGH);
      break;

    case 3:
      digitalWrite(RED, LOW);
      break;

    default:
      break;
  }
}

//detect PD signal//
void Op::signal_detect(int i) {
  switch (i % 4) {
    case 0:
      detected_NIR = analogRead(PD);
      NIR_buffer[i % 3] = detected_NIR;
      if (i > 3) {
        detected_NIR = (NIR_buffer[0] + NIR_buffer[1] + NIR_buffer[2]) / 3;
      }
      if (detected_NIR > 3700) {
        exceed = true;
      }
      else {
        exceed = false;
      }

      NIR_Signal.signal_analyze(detected_NIR, FIRfilter(detected_NIR, Coeffs_sample02, NIR_store), micros());
     
      break;

    case 2:
      detected_RED = analogRead(PD);
      RED_buffer[i % 3] = detected_RED;

      if (i > 3) {
        detected_RED = (RED_buffer[0] + RED_buffer[1] + RED_buffer[2]) / 3;
      }

      RED_Signal.signal_analyze(detected_RED, FIRfilter(detected_RED, Coeffs_sample02, RED_store), micros()); //store the data in the array//
     
      break;

    default:
      break;
  }
}

//Test for outputing//
void Op::signal_output(int i) {

  switch (i % 4) {
    case 0:
      Serial.print(NIR_Signal.get_currentdata());
      Serial.print(' ');
      break;
    case 2:
      Serial.print(RED_Signal.get_currentdata());
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
float Rarray[10] = {};
int R_cur = 0;
void Op::getHRSpO2() {
  NIR_Signal.signal_avr();
  RED_Signal.signal_avr();

  HR = NIR_Signal.heartrate;
  if (NIR_Signal.DCbase != 0 && RED_Signal.ACamp != 0) {
    float R = NIR_Signal.ACamp * RED_Signal.DCbase / NIR_Signal.DCbase / RED_Signal.ACamp;
    //Serial.println(R);
    Rarray[R_cur % 10] = R;
    R_cur++;
    if (R_cur >= 10) {
      R = avr(Rarray, 10, R_cur, 5);
    }
    if (R >= 1.15) {
      SpO2 = 99;
    }
    else if (R >= 0.85) {
      SpO2 = 99 - 5 * (1.15 - R) / 0.3 + 0.5;
    }
    else if (R > 0.3) {
      SpO2 = 95 - 45 * (0.85 - R) - 0.5;
    }
    else {
      SpO2 = 75 + R * 10;
    }
    SpO2 = (int)SpO2;
  }
}

Op op;
