/*----------------------------------Oscilloscope simulation-----------------------------------

  [Objective]
  Develop a functional oscilloscope on Arduino DUE with the following functions:
  1. Real-time signal sampling 
  2. Waveform display in serial ploter

  ** Fill in all [......] to make it functional**

---------------------------------------------------------------------------------------------*/



//==========================================================================================//
//                                1. Variable declaration                                   //
//==========================================================================================//

// Sampling frequency (Hz), set to 100 Hz for the case study //
const int loop_frequency = 100; 

// Sampling period (us) //
const int loop_period = 1000000 / loop_frequency;    

// Input channel (analog input) //
const int PD = A0;   

// Temporal reading of the input signal //
double Reading = 0;    

int loop_start = 0;
int loop_end = 0;


//==========================================================================================//
//                            2. Main program (initialize environment)                      //
//==========================================================================================//
void setup() {
  
  pinMode(PD,INPUT);
  // Turn on serial port (baud, bit per second) //
  Serial.begin(9600);

  // Set analog read resolution (bit) //
  analogReadResolution(12);



}



//==========================================================================================//
//                                 3. Main program (looping)                                //
//==========================================================================================//
void loop() {

  // Set variable to record start time for loop time monitoring (i.e sampling period) //
  loop_start = micros();

  // ======== Signal detection ======= //
  // Sampling at analog channel defined above //
  Reading = analogRead(PD);

  // Print the reading to serial port for visualization //  
  Serial.println(Reading);

  // Set variable to record end time for loop time monitoring (i.e sampling period) //
  loop_end = micros();

  // Set a time delay equal to the difference between start and end times to control loop time //
  delayMicroseconds(loop_start + loop_period - loop_end); 

}
