/*
 * This file includes the external constant and variable used in all of the other files.
*/
#define constant 1

#define PEAK 10;
#define TROUGH 11;

//constant declaration//
const int loop_frequency = 200;                     //loop frequency in Hz//
const int loop_period = 1000000 / loop_frequency;  //loop period in us//
const int PDdelay = loop_period / 2;                    //delay time for PD response in us//
const int erase_period = 10000000;          //Clear the buffer and calculate for every 10 seconds//


const int analog_MAX = 4095;                //Maximum of the analog reading/writing//
const double Vmax = 3.3;                    //Maximum of the output voltage//
const int MotorThreshold = 85;

// Pin declaration //
//const int RED = DAC0;  //Red LED//
//const int NIR = DAC1;  //NIR LED//
const int RED = A2;  //Red LED//
const int NIR = A1;  //NIR LED//
const int Motor = A3;
const int BlueTooth = A4;
const int VOUT = A7;

//const int RED = 5;
//const int NIR = 6;

const int PD = A0;  //photodetector//
float RED_buffer[3] = {};
float NIR_buffer[3] = {};


//Variable declaration//
int i = 0;           //loop times//
int loop_start = 0;  //starting time of this loop//
int loop_end = 0;    //ending time of this loop//
int detected_RED = 0;//reading of PD for RED signal//
int detected_NIR = 0;//reading of PD for NIR signal//
int filtered_RED = 0;//filtered data of RED signal//
int filtered_NIR = 0;//filtered data of NIR signal//
double Vout = 3.3;//Output woltage of LED light//
bool exceed = false;
bool BT = false;
//int LED[2] = {0,0};
int counter = 0;

//average for time//
float avr(float* data, int len, int startpoint, int slope){
  int totalweight = 0;
  float sum = 0.0;
  int z = len - slope;
  for(int c = 0; c < z; c++){
    sum += data[(startpoint + c) % len] * (c + 1);
    totalweight += c + 1;
  }
  for(int d = z; d < len; d++){
    sum += data[(startpoint + d) % len] * (z + 1);
    totalweight += z + 1;
  }
  return sum / totalweight;
}
