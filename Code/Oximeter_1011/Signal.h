/*
 * This file includes the defination of signal class
 */

#define signals 1

#ifndef constant
#include "Constant.h"
#endif

#ifndef point
#include "Point.h"
#endif


const int N_LEDarray = 11;                      //Length of the buffer signal array(must be odd)//
const int slide = 1;
const int heartrate_constrain[2] = {30,150};
int seg_threshold = 20;
int seg_avr[2] = {0,0};

class Signal
{
  private:
  int LEDcur = 0;
  bool label = true;    //True for exceeding threshold//
  //int seg_cur[2] = {0,0};
  //int tem_cur;
  Point seg_point[2];
  Point tem_point;
  int heartrate_threshold[2] = {30, 150}; 

  
  public:
  Point peakpoint;
  Point troughpoint;
  int state;

  float heartrate;
  Point LEDarray[N_LEDarray] = {};

  int signal_analyze(float data, int timestamp);
  int find_peak(int cur);
  int find_heartrate(int cur, int label);

  void cleardata();
  int getcur();
  void putdata(Point new_point);
  float get_currentdata();
  int realcur();


  
};

void Signal::cleardata(){
  LEDcur = 0;
  heartrate = 0.0;
  peakpoint.p_value = 0;
  peakpoint.p_timestamp = 0;
  troughpoint.p_value = 0;
  troughpoint.p_timestamp = 0;
  seg_point[0] = NULLPoint;
  seg_point[1] = NULLPoint;
  tem_point = NULLPoint;
  state = 0;
}

int Signal::getcur(){
  return LEDcur % N_LEDarray;
}

void Signal::putdata(Point p){
  LEDcur++;
  LEDarray[getcur()] = p;
}

float Signal::get_currentdata(){
  return LEDarray[getcur() % N_LEDarray].p_value;
}

int Signal::realcur(){
  return (LEDcur - (N_LEDarray / 2)) % N_LEDarray;
}

//analyze the signal data//
int Signal::signal_analyze(float data, int timestamp){
  if(data >= analog_MAX - 5)
    return -1;
  Point cur_point(data, timestamp);
  putdata(cur_point);

  state = find_peak(LEDcur);//determine whether this cursor points at a peak or trough//
  if(state == 1){
    peakpoint = LEDarray[realcur()];
    counter += 10;
  }
  else if(state == -1){
    troughpoint = LEDarray[realcur()];
    counter -= 10;
  }
  heartrate = find_heartrate(realcur(), state);
  return 0;
}


  //find peak, return 1 when finding peak, return -1 when finding trough//
int Signal::find_peak(int cur){
  if(cur < N_LEDarray) 
    return 0;

  float middle_data = LEDarray[(cur - N_LEDarray / 2) % N_LEDarray].p_value; //find the middle data in the buffer//
  int sign = 0;

  for(int j = 0; j < N_LEDarray; j += slide){
    float current_data = LEDarray[(cur - j) % N_LEDarray].p_value;
    if(middle_data == current_data) continue; //move to the next number when equal//
    if(sign == 0)
      sign = (middle_data > current_data)? 1 : -1; //set the sign//
      
    if(sign == 1 && middle_data < current_data)
      return 0;
      
    if(sign == -1 && middle_data > current_data)
      return 0;
      
  }
  return sign;
}

//Calculate the heartrate
int Signal::find_heartrate(int cur, int label){

    if(label == 0){
      return heartrate;
    }

    if(label == 1) {
      //seg_cur[0] = cur;
      seg_point[0] = LEDarray[cur % N_LEDarray];
      return heartrate;
    }

    if(label == -1 && seg_point[0].isNULL()){
      return heartrate;
    }
    
    if(label == -1 && tem_point.isNULL()){
      tem_point = LEDarray[cur % N_LEDarray];
      return heartrate;
    }



    seg_point[1] = LEDarray[cur % N_LEDarray];
    //float amp = LEDarray[seg_cur[0] % N_LEDarray].p_value - LEDarray[seg_cur[1] % N_LEDarray].p_value;
    float amp = seg_point[0].p_value - seg_point[1].p_value;
    seg_avr[0] += amp;
    seg_avr[1] += 1;
    //return amp;
    int heartperiod = seg_point[1].p_timestamp - tem_point.p_timestamp;
    float tem_heartrate = 60.0 * 1000000.0 / (float)heartperiod;
    //Serial.println(tem_heartrate);
    seg_threshold = seg_avr[0] / seg_avr[1] / 2;
    if(amp < seg_threshold || tem_heartrate < heartrate_constrain[0] || tem_heartrate > heartrate_constrain[1]){
      seg_point[0] = NULLPoint;
      tem_point = NULLPoint;
      return heartrate;
    }
    //int heartperiod = LEDarray[tem_cur % N_LEDarray].p_timestamp - LEDarray[cur % N_LEDarray].p_timestamp;

    tem_point = LEDarray[cur % N_LEDarray];

    return tem_heartrate;
}

Signal RED_Signal, NIR_Signal;
