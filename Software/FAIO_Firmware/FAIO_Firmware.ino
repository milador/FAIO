
//////////////////////////////////////
//
//FAIO ALL-IN-ONE switch
//
//Developed by Milad Hajihassan
//
//https://github.com/milador/FAIO
//
/////////////////////////////////////

#define MY_DEBUG 1
#define MY_TEST 0  //Testing mode = 0 , Production mode = 1

#include "BluefruitRoutines.h"

//Input in Assignment
#define ADAPTIVE_SWITCH1_PIN A4
#define ADAPTIVE_SWITCH2_PIN A5
#define SIP_PUFF_SWITCH_PIN A7

#define BRAILLE_BUTTON11_PIN A1
#define BRAILLE_BUTTON12_PIN 10
#define BRAILLE_BUTTON21_PIN A2
#define BRAILLE_BUTTON22_PIN 9
#define BRAILLE_BUTTON31_PIN A3
#define BRAILLE_BUTTON32_PIN A0

#define LED1_PIN 11
#define LED2_PIN 13

// Variable Declaration 
int adaptiveSwitch1State;
int adaptiveSwitch2State;
float sipPuffPressureValue = 0.00;

float sipPressureThreshold;
float puffPressureThreshold;

int adaptiveSwitch1Enabled = 0;
int adaptiveSwitch2Enabled = 0;
int sipPuffPressureEnabled = 0;

//BRAILLE_BUTTON brailleButton
int brailleButton11State;
int brailleButton12State;
int brailleButton21State;
int brailleButton22State;
int brailleButton31State;
int brailleButton32State;

int braillePadEnabled = 0;
int brailleButton11Enabled = 0;
int brailleButton12Enabled = 0;
int brailleButton21Enabled = 0;
int brailleButton22Enabled = 0;
int brailleButton31Enabled = 0;
int brailleButton32Enabled = 0;

////////////////////////////
//Function to ranslate character to keyboard keycode and transmit
///////////////////////////

void pressKeyCode (uint8_t c) {
  ble.print(F("AT+BLEKEYBOARDCODE=00-00-"));
  uint8_t Code=c-'a'+4;
  if (Code<0x10)ble.print("0");
  ble.print(Code,HEX);
  ble.println(F("-00-00-00-00"));
  MESSAGE(F("Pressed."));
  delay(100);//de-bounce
  ble.println(F("AT+BLEKEYBOARDCODE=00-00"));
  MESSAGE(F("Released"));
}

////////////////////////////
//setup function
///////////////////////////

void setup() {
  //Initialize serial communications at 9600 bps
  Serial.begin(9600);
  pinMode(ADAPTIVE_SWITCH1_PIN, INPUT_PULLUP);
  pinMode(ADAPTIVE_SWITCH2_PIN, INPUT_PULLUP);
  pinMode(SIP_PUFF_SWITCH_PIN, INPUT);
  
  pinMode(BRAILLE_BUTTON11_PIN, INPUT_PULLUP);
  pinMode(BRAILLE_BUTTON12_PIN, INPUT_PULLUP);
  pinMode(BRAILLE_BUTTON21_PIN, INPUT_PULLUP);
  pinMode(BRAILLE_BUTTON22_PIN, INPUT_PULLUP);
  pinMode(BRAILLE_BUTTON31_PIN, INPUT_PULLUP);
  pinMode(BRAILLE_BUTTON32_PIN, INPUT_PULLUP);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT); 
  
  initializeBluefruit();
  initialization();
}

////////////////////////////
//LED blink function
///////////////////////////

void ledBlink(int numberBlinks, int delayBlinks, int ledNumber ) {
      int ledBlinkNumber = ledNumber;
      for (int i = 0; i < numberBlinks; i++) {
        digitalWrite(ledBlinkNumber, HIGH);
        delay(delayBlinks);
        digitalWrite(ledBlinkNumber, LOW);
        delay(delayBlinks);
      }
}

////////////////////////////
//Initialization function
///////////////////////////

void initialization() {

   delay(500);
   
    //LED blinks to indicate start of initialization process
    ledBlink(2,100,LED1_PIN);
    ledBlink(2,100,LED2_PIN);

    //Initialization start message
    Serial.println("FAIO switch Initialization started.");
    Serial.println("----------");
    //adaptive switch initialization process to check if the switches are installed
    adaptiveSwitch1Enabled = digitalRead(ADAPTIVE_SWITCH1_PIN);
    if (adaptiveSwitch1Enabled == HIGH)
    {
      Serial.println("Adaptive Switch 1 is enabled.");
    }
    else
    {
      Serial.println("Adaptive Switch 1 is disabled.");
    }
    adaptiveSwitch2Enabled = digitalRead(ADAPTIVE_SWITCH2_PIN);
    if (adaptiveSwitch2Enabled == HIGH)
    {
      Serial.println("Adaptive Switch 2 is enabled.");
    }
    else
    {
      Serial.println("Adaptive Switch 2 is disabled.");
    }

    //Braille button initialization process to check if the Braille is installed
    brailleButton11Enabled = digitalRead(BRAILLE_BUTTON11_PIN);
    brailleButton12Enabled = digitalRead(BRAILLE_BUTTON12_PIN);
    brailleButton21Enabled = digitalRead(BRAILLE_BUTTON21_PIN);
    brailleButton22Enabled = digitalRead(BRAILLE_BUTTON22_PIN);
    brailleButton31Enabled = digitalRead(BRAILLE_BUTTON31_PIN);
    brailleButton32Enabled = digitalRead(BRAILLE_BUTTON32_PIN);
    if (brailleButton11Enabled && brailleButton12Enabled && brailleButton21Enabled && brailleButton22Enabled && brailleButton31Enabled && brailleButton32Enabled)
    {
      braillePadEnabled=1;
      Serial.println("Braille Pad is installed.");
    }
    else
    {
      braillePadEnabled=0;
      Serial.println("Braille Pad is not installed.");
    }
    
    //Pressure Sensor initialization process
    float normalPressureValue = (((float)analogRead(SIP_PUFF_SWITCH_PIN)) / 1023.0) * 3.30;
    //Check if Pressure Sensor is installed 
    if (normalPressureValue != 0.00)
    {
      //Find sip and puff pressure thresholds and show them 
      sipPuffPressureEnabled=1;
      sipPressureThreshold = normalPressureValue + 0.4; 
      puffPressureThreshold = normalPressureValue - 0.4; 
      Serial.print("Pressure Sensor Initialization completed. ");
      Serial.print("Sip Pressure Threshold is "); 
      Serial.print(sipPressureThreshold); 
      Serial.print(" And Puff Pressure Threshold is "); 
      Serial.println(puffPressureThreshold);    
    }
    else
    {
      sipPuffPressureEnabled=0;
      Serial.println("Pressure Sensor is not installed.");
    }
    
    //Initialization end message
    Serial.println("----------");
    Serial.println("FAIO switch initialization ended.");

    //LED blinks to indicate end of initialization process 
    ledBlink(2,100,LED1_PIN);
    ledBlink(2,100,LED2_PIN);
   
    delay(10000);
}

////////////////////////////
//Braille function
///////////////////////////

char braillePad(int pin11, int pin12, int pin21, int pin22, int pin31, int pin32) {
    char brailleChar;
    pin11=!pin11;
    pin12=!pin12;
    pin21=!pin21;
    pin22=!pin22;
    pin31=!pin31;
    pin32=!pin32;    
    if(pin11==1 && pin12==0 && pin21==0 && pin22==0 && pin31==0 && pin32==0) { brailleChar='A'; }
    else if(pin11==1 && pin12==0 && pin21==1 && pin22==0 && pin31==0 && pin32==0) { brailleChar='B'; }
    else if(pin11==1 && pin12==1 && pin21==0 && pin22==0 && pin31==0 && pin32==0) { brailleChar='C'; }
    else if(pin11==1 && pin12==1 && pin21==0 && pin22==1 && pin31==0 && pin32==0) { brailleChar='D'; }
    else if(pin11==1 && pin12==0 && pin21==0 && pin22==1 && pin31==0 && pin32==0) { brailleChar='E'; }
    else if(pin11==1 && pin12==1 && pin21==1 && pin22==0 && pin31==0 && pin32==0) { brailleChar='F'; }
    else if(pin11==1 && pin12==1 && pin21==1 && pin22==1 && pin31==0 && pin32==0) { brailleChar='G'; }
    else if(pin11==1 && pin12==0 && pin21==1 && pin22==1 && pin31==0 && pin32==0) { brailleChar='H'; }
    else if(pin11==0 && pin12==1 && pin21==1 && pin22==0 && pin31==0 && pin32==0) { brailleChar='I'; }
    else if(pin11==0 && pin12==1 && pin21==1 && pin22==1 && pin31==0 && pin32==0) { brailleChar='J'; }
    else if(pin11==1 && pin12==0 && pin21==0 && pin22==0 && pin31==1 && pin32==0) { brailleChar='K'; }
    else if(pin11==1 && pin12==0 && pin21==1 && pin22==0 && pin31==1 && pin32==0) { brailleChar='L'; }
    else if(pin11==1 && pin12==1 && pin21==0 && pin22==0 && pin31==1 && pin32==0) { brailleChar='M'; }
    else if(pin11==1 && pin12==1 && pin21==0 && pin22==1 && pin31==1 && pin32==0) { brailleChar='N'; }
    else if(pin11==1 && pin12==0 && pin21==0 && pin22==1 && pin31==1 && pin32==0) { brailleChar='O'; }
    else if(pin11==1 && pin12==1 && pin21==1 && pin22==0 && pin31==1 && pin32==0) { brailleChar='P'; }
    else if(pin11==1 && pin12==1 && pin21==1 && pin22==1 && pin31==1 && pin32==0) { brailleChar='Q'; }
    else if(pin11==1 && pin12==0 && pin21==1 && pin22==1 && pin31==1 && pin32==0) { brailleChar='R'; }
    else if(pin11==0 && pin12==1 && pin21==1 && pin22==0 && pin31==1 && pin32==0) { brailleChar='S'; }     
    else if(pin11==0 && pin12==1 && pin21==1 && pin22==1 && pin31==1 && pin32==0) { brailleChar='T'; }
    else if(pin11==1 && pin12==0 && pin21==0 && pin22==0 && pin31==1 && pin32==1) { brailleChar='U'; }
    else if(pin11==1 && pin12==0 && pin21==1 && pin22==0 && pin31==1 && pin32==1) { brailleChar='V'; }
    else if(pin11==0 && pin12==1 && pin21==1 && pin22==1 && pin31==0 && pin32==1) { brailleChar='W'; }
    else if(pin11==1 && pin12==1 && pin21==0 && pin22==0 && pin31==1 && pin32==1) { brailleChar='X'; }
    else if(pin11==1 && pin12==1 && pin21==0 && pin22==1 && pin31==1 && pin32==1) { brailleChar='Y'; }
    else if(pin11==1 && pin12==0 && pin21==0 && pin22==1 && pin31==1 && pin32==1) { brailleChar='Z'; }
    else if(pin11==0 && pin12==1 && pin21==0 && pin22==1 && pin31==1 && pin32==1) { brailleChar='#'; }
               
    return brailleChar;
}

////////////////////////////
//Main loop function
///////////////////////////

void loop() {
    //Read input pin values
    adaptiveSwitch1State = digitalRead(ADAPTIVE_SWITCH1_PIN);
    adaptiveSwitch2State = digitalRead(ADAPTIVE_SWITCH2_PIN);
    sipPuffPressureValue = (((float)analogRead(SIP_PUFF_SWITCH_PIN)) / 1023.0) * 3.30;

    brailleButton11State = digitalRead(BRAILLE_BUTTON11_PIN);
    brailleButton12State = digitalRead(BRAILLE_BUTTON12_PIN);
    brailleButton21State = digitalRead(BRAILLE_BUTTON21_PIN);
    brailleButton22State = digitalRead(BRAILLE_BUTTON22_PIN);
    brailleButton31State = digitalRead(BRAILLE_BUTTON31_PIN);
    brailleButton32State = digitalRead(BRAILLE_BUTTON32_PIN);

    (brailleButton11State && brailleButton12State && brailleButton21State && brailleButton22State && brailleButton31State && brailleButton32State)  ? braillePadEnabled=1 : braillePadEnabled=0;
    (adaptiveSwitch1State) ? adaptiveSwitch1Enabled=1 : adaptiveSwitch1Enabled=0;
    (adaptiveSwitch2State) ? adaptiveSwitch2Enabled=1 : adaptiveSwitch2Enabled=0;

    
  //Testing mode
  if(MY_TEST==0){
    
    Serial.print("Switch1 = ");
    Serial.print(adaptiveSwitch1State);
    Serial.print(" Switch2 = ");
    Serial.println(adaptiveSwitch2State);

    
    Serial.print("Braille Character = ");
    Serial.println(braillePad(brailleButton11State,brailleButton12State,brailleButton21State,brailleButton22State,brailleButton31State,brailleButton32State));
    
    
    Serial.print("Sip Puff Switch Value = ");
    Serial.println(sipPuffPressureValue);
    delay(500);
  }
  //Production mode
  else 
  {
    //Adaptive Switch 1 Activation command
    if(adaptiveSwitch1Enabled && !adaptiveSwitch1State)
      {
        Serial.println("Send Adaptive Switch 1 Activation command.");
        ledBlink(1,100,LED1_PIN);
        //pressKeyCode('1');
        ledBlink(1,100,LED2_PIN);
      }
      
     //Adaptive Switch 2 Activation command
     if(adaptiveSwitch2Enabled && !adaptiveSwitch2State)
      {
        Serial.println("Send Adaptive Switch 2 Activation command.");
        ledBlink(1,100,LED1_PIN);
        //pressKeyCode('2');
        ledBlink(1,100,LED2_PIN);
      }

      //Sip andvPuff Pressure Activation command
      if(sipPuffPressureEnabled)
      {
        if(sipPuffPressureValue>sipPressureThreshold)
        {
          //Sip Activation command
          ledBlink(1,100,LED1_PIN);
          Serial.println("Send Sip Activation command.");
          //pressKeyCode('3');
          ledBlink(1,100,LED2_PIN);
        }
        else if (sipPuffPressureValue<puffPressureThreshold) 
        {
          //Puff Activation command
          ledBlink(1,100,LED1_PIN);
          Serial.println("Send Puff Activation command.");
          //pressKeyCode('4');
          ledBlink(1,100,LED2_PIN);
        }
        else
        {
          //Do nothing 
        }
        
    }

    //Send Braille character 
     if(braillePadEnabled==HIGH)
      {
        if(!(brailleButton11State && brailleButton12State && brailleButton21State && brailleButton22State && brailleButton31State && brailleButton32State))
        {
          ledBlink(2,100,LED1_PIN);
          Serial.print("Send Braille characters : ");
          Serial.println(braillePad(brailleButton11State,brailleButton12State,brailleButton21State,brailleButton22State,brailleButton31State,brailleButton32State));
         //pressKeyCode(braillePad(brailleButton11State,brailleButton12State,brailleButton21State,brailleButton22State,brailleButton31State,brailleButton32State));
          ledBlink(2,100,LED2_PIN);
        }
      }     
      if( !ble.waitForOK() )
      {
        //Serial.println( F("FAILED!") );
      }
      delay(500);
  }
}

