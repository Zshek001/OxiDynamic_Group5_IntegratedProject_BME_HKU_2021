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

#ifndef filter
#include "Filter.h"
#endif


const int N_LEDarray = 21;                      //Length of the buffer signal array(must be odd)//
const int slide = 1;
const int heartrate_constrain[2] = {30,150};
int seg_threshold = 5;
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
  float Rawarray[N_LEDarray] = {};
  
  float DCarray[10] = {};
  int DC_cur = 0;
  float HRarray[10] = {};
  int HR_cur = 0;
  float ACarray[10] = {};  
  int AC_cur = 0;

  
  public:
  Point peakpoint;
  Point troughpoint;
  int state;
  float ACamp;
  float DCbase;

  float heartrate = 0.0;
  Point LEDarray[N_LEDarray] = {};


  int signal_analyze(float rawdata, float data, int timestamp);
  int find_peak(int cur);
  int find_heartrate(int cur, int label);

  void cleardata();
  void initdata();
  int getcur();
  void putdata(Point new_point);
  float get_currentdata();
  int realcur();
  void signal_avr();

  
};

void Signal::signal_avr(){
  if(DC_cur < 10){
    DCbase = DCarray[DC_cur];
  }
  else{
    DCbase = avr(DCarray, 10, DC_cur, 5);
  }
  if(AC_cur < 10){
    ACamp = ACarray[AC_cur];
  }
  else{
    ACamp = avr(ACarray, 10, AC_cur, 5);
  }
  if(HR_cur < 10){
    heartrate = HRarray[HR_cur];
  }
  else{
    heartrate = avr(HRarray, 10, HR_cur, 5);
  }
}

void Signal::cleardata(){
  LEDcur = 0;
  //heartrate = 0.0;
  //peakpoint.p_value = 0;
  //peakpoint.p_timestamp = 0;
  //troughpoint.p_value = 0;
  //troughpoint.p_timestamp = 0;
  seg_point[0] = NULLPoint;
  seg_point[1] = NULLPoint;
  //tem_point = NULLPoint;
  state = 0;
}

void Signal::initdata(){
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
  DC_cur = 0;
  HR_cur = 0;
  AC_cur = 0;
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
int Signal::signal_analyze(float rawdata, float data, int timestamp){

  Point cur_point(data, timestamp);
  putdata(cur_point);
  Rawarray[getcur()] = rawdata;

  state = find_peak(LEDcur);//determine whether this cursor points at a peak or trough//
  if(state == 1){
    peakpoint = LEDarray[realcur()];
    counter += 10;
  }
  else if(state == -1){
    troughpoint = LEDarray[realcur()];
    DCarray[DC_cur % 10] = Rawarray[realcur()];
    DC_cur++;
    counter -= 10;
  }
  find_heartrate(realcur(), state);
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
    float amp = seg_point[0].p_value - seg_point[1].p_value;
    seg_avr[0] += amp;
    seg_avr[1] += 1;
    int heartperiod = seg_point[1].p_timestamp - tem_point.p_timestamp;
    float tem_heartrate = 60.0 * 1000000.0 / (float)heartperiod;
    if(amp < seg_threshold){
      seg_point[0] = NULLPoint;
      tem_point = NULLPoint;
      return heartrate;
    }
    ACarray[AC_cur % 10] = amp;
    AC_cur++;
    if(tem_heartrate < heartrate_constrain[0] || tem_heartrate > heartrate_constrain[1]){
      seg_point[0] = NULLPoint;
      tem_point = NULLPoint;
      return heartrate;
    }

    tem_point = LEDarray[cur % N_LEDarray];
    
    HRarray[HR_cur % 10] = tem_heartrate;
    HR_cur++;

    return tem_heartrate;
}

Signal RED_Signal, NIR_Signal;
