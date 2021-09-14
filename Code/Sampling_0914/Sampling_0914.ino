


//constant declaration//
const int loop_frequency = 80;                     //loop frequency in Hz//
const int loop_period = 1000000 / loop_frequency;  //loop period in us//
const int PDdelay = 7500;                     //delay time for PD response in us//
const int erase_period = 5000000;          //Clear the buffer and calculate for every 5 seconds//
const int N_LEDarray = 200;                      //Length of the detected signal array//

const int analog_MAX = 4095;                //Maximum of the analog reading/writing//
const double Vmax = 3.3;                    //Maximum of the output voltage//

// Pin declaration //
const int RED = DAC0;  //Red LED//
const int NIR = DAC1;  //NIR LED//
const int PD = A0;  //photodetector//



//Variable declaration//
int i = 0;           //loop times//
int loop_start = 0;  //starting time of this loop//
int loop_end = 0;    //ending time of this loop//
int detected_RED = 0;//reading of PD for RED signal//
int detected_NIR = 0;//reading of PD for NIR signal//
int filtered_RED = 0;//filtered data of RED signal//
int filtered_NIR = 0;//filtered data of NIR signal//
double Vout = 3.3;//Output woltage of LED light//
//int LED[2] = {0,0};



//Declare of objects//
class Signal
{
  public: double maxnum;
  public: double minnum;
  public: double heartrate;

  //Declare the array for store the detected LED signal and its curve//

  double LEDarray[N_LEDarray] = {};
  int LEDcur = 0;
  bool label = true;    //True for exceeding threshold//
  int tem_cur = 0;           //store the last cursor exceeding the threshold//

  public: int signal_analyze(double current){

    
    if(find_peak(current) == 0){
      heartrate = 60000000 / (4 * loop_period * find_period(current));
      return 0;
    }
    else
      return -1;
  }
  
  int find_peak(double current){
    if(current > maxnum) 
      maxnum = current;
    if(current < minnum) 
      minnum = current;

    if(maxnum == 0 || minnum == analog_MAX) 
      return -1;
    else 
      return 0;
  }
  
  int find_period(double current){
    int period = 0;
    
    if(maxnum == 0 || minnum == 0 || LEDcur < N_LEDarray)
      return -1;
    int threshold = (maxnum + minnum) / 2;

    if(label == true && current < threshold - (threshold - minnum) * 0.4)
      label = false;
    else if(label == false && current > threshold + (maxnum - threshold)){
      label = true;
      if(tem_cur != 0 && LEDcur != 0)
        period = LEDcur - tem_cur;
      tem_cur = LEDcur;
    }

    return period;

    
  }

  public: void cleardata(){
    maxnum = 0;
    minnum = analog_MAX;
    heartrate = 0;
    label = true;
  }

  public: int getcur(){
    return LEDcur % N_LEDarray;
  }

  public: void putdata(double current){
    LEDcur++;
    LEDarray[getcur()] = current;
  }

  public: double get_currentdata(){
    return LEDarray[getcur() % N_LEDarray];
  }

  public: void movecur(){
    LEDcur++;
  }
};

Signal RED_Signal, NIR_Signal;


       

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  //Setup the pinmode//
  pinMode(RED,OUTPUT);
  pinMode(NIR,OUTPUT);
  pinMode(PD,INPUT);

  // Set the resolution(0 - 4095)//
  analogReadResolution(12);
  analogWriteResolution(12);

}

void loop() {
  // put your main code here, to run repeatedly:
    
  loop_start = micros();  //Record the start time of the loop//

  //generate PD signal//
  switch (i % 4){
    case 0:
    analogWrite(NIR,(Vout) * analog_MAX / Vmax);
    //LED[0] = 1;
    break;

    case 1:
    analogWrite(NIR,0);
    //LED[0] = 0;
    break;

    case 2:
    analogWrite(RED, Vout * analog_MAX / Vmax);
    //LED[1] = 10;
    break;

    case 3:
    analogWrite(RED,0);
    //LED[1] = 0;
    break;

    default:
    break;
  }

  //detect PD signal//
   delayMicroseconds(loop_start + PDdelay - micros());
    switch (i % 4){
      case 0:
      detected_NIR = analogRead(PD);
      NIR_Signal.putdata(detected_NIR); //store the data in the array//

      //detected_NIR = LED[0] + LED[1];
      break;

      case 2:
      detected_RED = analogRead(PD);
      RED_Signal.putdata(detected_RED); //store the data in the array//
      //if (detected_RED > (analog_MAX * 0.9)){
      //  Vout -= 0.1;
    //  }
      //detected_RED = LED[1] + LED[0];
      break;
      
      default:
      break;
    }
    
//Test for outputing//
  RED_Signal.signal_analyze(RED_Signal.get_currentdata());
  Serial.print(RED_Signal.get_currentdata());
  Serial.print(' ');
  //Serial.print(RED_Signal.maxnum);
  //Serial.print(' ');
 // Serial.print(RED_Signal.minnum);
  Serial.print(' ');
  Serial.println(NIR_Signal.get_currentdata());

  i++;

  //delay to fix the loop time//
  //if(i >= erase_period / loop_period){
  //  RED_Signal.cleardata();
 //   NIR_Signal.cleardata();
  //  i = 0;

 // }
   if(i > 1000)
    while(1);
  loop_end = micros();
  if(loop_start + loop_period - loop_end > 0) delayMicroseconds(loop_start + loop_period - loop_end);
  else Serial.println("Error");
}
