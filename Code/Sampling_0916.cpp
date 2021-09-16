#include "filter0916.h"

//constant declaration//
const int loop_frequency = 200;                    //loop frequency in Hz//
const int loop_period = 1000000 / loop_frequency;  //loop period in us//
const int PDdelay = loop_period / 2;               //delay time for PD response in us//
const int erase_period = 5000000;                  //Clear the buffer and calculate for every 5 seconds//
const int N_LEDarray = 200;                       //Length of the detected signal array//

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
//**********************************filter************************************//
const unsigned int CASC_SECTION 13;

static float iir_state[4*CASC_SECTION];
const float iir_coeffs[5*CASC_SECTION] = {
  1,  -1.996106048410463351672206044895574450493   ,1,    -0.953038797436169149435158942651469260454,  0.88951701622260170854161742681753821671,
  1,  -0.607585479972089892441999836592003703117   ,1,    -1.983035821437771240027814201312139630318,  0.989309783362369521420021101221209391952,
  1,  -1.996512834973201533372844096447806805372   ,1,    -0.821569753291357507407610683003440499306,  0.691414523826323734745358251529978588223,
  1,  -0.505735519260496202420540612365584820509   ,1,    -1.962284641863488943158699839841574430466,  0.968193137133703474184187598439166322351,
  1,  -1.997252134432572212574541481444612145424   ,1,    -1.942143793607569612902352673700079321861,  0.947445567949532274631962991406908258796,
  1,  -0.276712392731898459974360093838186003268   ,1,    -0.674233199860213572307543472561519593   ,  0.506828634663675448024378056288696825504,
  1,  -1.998177986015588158963396381295751780272   ,1,    -1.922989148834448602798374849953688681126,  0.927546550595506613667851070204051211476,
  1,   0.132420956608992362868448822155187372118   ,1,    -0.517681159762837905802257409959565848112,  0.329959529815342933467547936743358150125,
  1,  -1.999086646198828365328381551080383360386   ,1,    -1.906058267410661866136933895177207887173,  0.909881544787440876831396963098086416721,
  1,   0.77984447016315450174062107180361635983    ,1,    -0.370497944605992945987793518725084140897,  0.17111404969930016362056335310626309365,
  1,  -1.999754154672984896023990586400032043457   ,1,    -1.893775738603643787882901960983872413635,  0.897050902152532025901621182129019871354,
  1,   1.577414675012146938470891655015293508768   ,1,    -0.261989448254461798626380186760798096657,  0.055403092225131220227574146974802715704,
  1,   0                                          ,-1,    -1.055383004818446046613189537310972809792,  0.104655349465019156807166211820003809407
};
const float iir_gain[CASC_SECTION] = {
  0.795583608836923139584484943043207749724,
  0.795583608836923139584484943043207749724,
  0.739707446968585702329335163085488602519,
  0.739707446968585702329335163085488602519,
  0.676649493843606286702652141684666275978,
  0.676649493843606286702652141684666275978,
  0.605860250279752321489468158688396215439,
  0.605860250279752321489468158688396215439,
  0.532669526815833349608908520167460665107,
  0.532669526815833349608908520167460665107,
  0.47218929343133497766871187195647507906,
  0.47218929343133497766871187195647507906,
  0.447672325267490400779735182368312962353
};

//****************************************************************************//


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

 filter_main(
   CASC_SECTION,
   *iir_state,
   *iir_coeffs,
   *iir_gain,
   *RED_Signal.LEDarray,
   *output???,
   N_LEDarray
 )


   if(i > 1000)
    while(1);
  loop_end = micros();
  if(loop_start + loop_period - loop_end > 0) delayMicroseconds(loop_start + loop_period - loop_end);
  else Serial.println("Error");
}
