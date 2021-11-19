
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

#include <ArduinoBLE.h>

// BLE Battery Service
BLEService OxiDynamic("19B10000-E8F2-537E-4F6C-D104768A1214");

// BLE Battery Level Characteristic
BLEUnsignedCharCharacteristic BT_HR("2A190000-0000-0000-0000-000000000000",  // standard 16-bit characteristic UUID
                                    BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes
BLEUnsignedCharCharacteristic BT_SpO2("2A200000-0000-0000-0000-000000000000", BLERead | BLENotify);

long previousMillis = 0;
void updateHRSpO2();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Setup the pinmode//
  pinMode(PD, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(NIR, OUTPUT);
  pinMode(VOUT, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  // Set the resolution(0 - 4095)//
  analogReadResolution(12);
  analogWriteResolution(12);

//init the signal class and screen//
  RED_Signal.initdata();
  NIR_Signal.initdata();
  OLEDinit();
  OLEDtitle();
}

void loop() {
  BLEDevice central = BLE.central();

//if the BT switch is on, turn on the BLE module//
  if ((!BT) && analogRead(BlueTooth) >= 1000) {
    //Serial.println("ss");
    BLE.begin();
    BLE.setLocalName("OxiDynamic");
    BLE.setAdvertisedService(OxiDynamic); // add the service UUID
    OxiDynamic.addCharacteristic(BT_HR); // add the battery level characteristic
    OxiDynamic.addCharacteristic(BT_SpO2);
    BLE.addService(OxiDynamic); // Add the battery service
    BLE.advertise();
    BT = true;
  }
  else if (BT && analogRead(BlueTooth) < 1000) {
    BLE.stopAdvertise();
    BLE.end();
    BT = false;
  }
  digitalWrite(VOUT, HIGH);

  // put your main code here, to run repeatedly:
  loop_start = micros();  //Record the start time of the loop//
  op.signal_generate(i);
  if (i % 2 == 0) {
    delayMicroseconds(loop_start + PDdelay - micros());
    //detect the signal and print the waveform to the Serial port//
    op.signal_detect(i);
    op.signal_output(i);
  }
  else if (i % 4 == 3) {//calculate the HR and SpO2 at the end of each period//
    op.getHRSpO2();
  }


//display the data per 5s//
  if (i % 1001 == 0) {
    if (exceed) { //if the signak exceed the threshold, diaplay error information//
      OLEDError();
      if (BT) {
        OLEDBTON();
      }
      else {
        OLEDBTOFF();
      }
    }
    else {
      OLEDheartrate(op.HR);
      OLEDR(op.SpO2);
      if (BT) {
        if (central.connected()) {

          updateHRSpO2();
        }
      }

    }
    OLED.display();

//clear the buffer//
    RED_Signal.cleardata();
    NIR_Signal.cleardata();
  }
  
//drive the motor if the SpO2 is too low//
  if (op.SpO2 < MotorThreshold) {
    op.MotorOn(i);
  }
  else {
    op.MotorOff();
  }

  i++;

  //delay to average the period of each loop(5ms)//
  loop_end = micros();
  if (loop_start + loop_period - loop_end > 0) delayMicroseconds(loop_start + loop_period - loop_end);

}


void updateHRSpO2() {
  /* Upload the current data to the mobile APP through BlueTooth
  */
  BT_HR.writeValue(op.HR - 30);  //update the HR and SpO2 characteristic
  BT_SpO2.writeValue(op.SpO2);
}
