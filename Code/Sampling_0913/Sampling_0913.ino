


//constant declaration//
const int loop_frequency = 80;                     //loop frequency in Hz//
const int loop_period = 1000000 / loop_frequency;  //loop period in us//
const int PDdelay = 7500;                     //delay time for PD response in us//
const int N_LED = 200;

// Pin declaration //
const int RED = 2;  //Red LED//
const int NIR = 3;  //NIR LED//
const int PD = A0;  //photodetector//



//Variable declaration//
int i = 0;           //loop times//
int loop_start = 0;  //starting time of this loop//
int loop_end = 0;    //ending time of this loop//
int detected_RED = 0;//reading of PD for RED signal//
int detected_NIR = 0;//reading of PD for NIR signal//
int filtered_RED = 0;//filtered data of RED signal//
int filtered_NIR = 0;//filtered data of NIR signal//
//int LED[2] = {0,0};

//Declare the array for store the detected LED signal and its curve//
int REDarray[N_LED] = {};
int NIRarray[N_LED] = {};
int REDcur = 0;
int NIRcur = 0;
       

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
    digitalWrite(NIR,HIGH);
    //LED[0] = 1;
    break;

    case 1:
    digitalWrite(NIR,LOW);
    //LED[0] = 0;
    break;

    case 2:
    digitalWrite(RED,HIGH);
    //LED[1] = 10;
    break;

    case 3:
    digitalWrite(RED,LOW);
    //LED[1] = 0;
    break;

    default:
    break;
  }

  //detect PD signal//
   delayMicroseconds(loop_start + PDdelay - micros());
    switch (i % 4){
      case 1:
      detected_NIR = analogRead(PD);
      NIRarray[NIRcur++ % N_LED] = detected_NIR; //store the data in the array//
      //detected_NIR = LED[0] + LED[1];
      break;

      case 3:
      detected_RED = analogRead(PD);
      REDarray[REDcur++ % N_LED] = detected_RED; //store the data in the array//
      //detected_RED = LED[1] + LED[0];
      break;
      
      default:
      break;
    }
    
//Test for outputing//
  Serial.print(detected_NIR);
  Serial.print(' ');
  Serial.println(detected_RED);
  i++;

  //delay to fix the loop time//
  loop_end = micros();
  if(loop_start + loop_period - loop_end > 0) delayMicroseconds(loop_start + loop_period - loop_end);
  else Serial.println("Error");
}
