// #include <LiquidCrystal_I2C.h>

#include <SoftwareSerial.h>
#include "motors.h"


#define CLOSE_LIMIT_PIN 8
#define OPEN_LIMIT_PIN 9
#define STEP_IN1 10
#define STEP_IN2 11
#define STEP_IN3 12
#define STEP_IN4 13


#define OPEN_DOOR_PIN 6
#define BUZZER_PIN 7
#define BUZZER_LENGTH 3000UL
#define MAX_TAG_COUNT 5

Stepper_28BYJ_48 DoorMotor;
//LiquidCrystal_I2C Lcd(0x27,16,2);
//int DuncanCount=0;

//const unsigned long ValidTags[MAX_TAG_COUNT]={10131965UL,10418284UL,0UL,0UL,0UL};
const unsigned long ValidTags[MAX_TAG_COUNT]={0UL,0UL,0UL,0UL,0UL};

SoftwareSerial RFID(2,3); //Rx,Tx pins
const unsigned long ClosedDelay = 120000UL; // 2 minutes
//const unsigned long ClosedDelay = 7000UL; // 7 sec


void clearRFIDBuffer(){
  while(RFID.available()>0){
    RFID.read();
   }
}

unsigned long checkRFID(){
  int tagData;
  int tagID[14];
  int index=0;
  char data[8];
  unsigned long x=0UL;
  if(RFID.available()>0){
    //Serial.println("Found Tag");
    for(index=0;index<14;++index){
      while(RFID.available()<=0){}
      tagData=RFID.read();
      tagID[index]=tagData;
      //delay(100);
      //Serial.print(tagData,DEC);Serial.print(" ");
      
    }
    //Serial.println();
    data[0]=(char)tagID[3]; data[1]=(char)tagID[4];
    data[2]=(char)tagID[5]; data[3]=(char)tagID[6];
    data[4]=(char)tagID[7]; data[5]=(char)tagID[8];
    data[6]=(char)tagID[9]; data[7]=(char)tagID[10];
    x=strtoul(data,0,16);
    //Serial.print(data); Serial.print(" "); Serial.println(x);
    clearRFIDBuffer();
    }
  return(x);
}

boolean activateDoor(int value){
  boolean isDoorClosed=false;
  if(value==1){ // close door
      //Lcd.setCursor(8,0);
      //Lcd.print(DuncanCount);
      enableBuzzer(1);
      DoorMotor.setStepperDirection(1);
      DoorMotor.moveToCcwLimit();
      enableBuzzer(0);
      isDoorClosed=true;
  }else{

      DoorMotor.setStepperDirection(0);
      DoorMotor.moveToCwLimit();;
  }
  return(isDoorClosed);
}

void enableBuzzer(int value){
  if(value == 0){
    digitalWrite(BUZZER_PIN,LOW);
  }else{
    digitalWrite(BUZZER_PIN,HIGH);
  }
}
void checkBuzzer(unsigned long myTime){
  static int buzzerOn=0;

  if(myTime>0UL){
    if(buzzerOn==0){
    //Serial.print("in Buzzer: "); Serial.print(buzzerOn);Serial.println(" ");Serial.println(myTime);
    }
    if(buzzerOn == 1 && myTime >= BUZZER_LENGTH){
      digitalWrite(BUZZER_PIN,LOW);
      //Serial.println("buzzer off");
      buzzerOn=0;
    }else if(buzzerOn && myTime< BUZZER_LENGTH){
      digitalWrite(BUZZER_PIN,HIGH);
      //Serial.println("buzzer on");
      buzzerOn=1;
    }
  }
}

boolean isValidTag(unsigned long tagID){
  int index=0,counter=0;
  boolean result=false;
  for(index=0;index<MAX_TAG_COUNT;++index){
    if(ValidTags[index]==0){
      ++counter;  
    }else if(ValidTags[index] == tagID){
      result=true;
      break;
    }
  }
  if(counter == MAX_TAG_COUNT){ // if all ValidTags equal 0 (no tag ID's specified)
    result=true;
  }
  return(result);
}

void setup() {
  RFID.begin(9600);
  Serial.begin(9600);
//  Lcd.init();
//  Lcd.clear();
//  Lcd.backlight();
//  Lcd.print("Number:");
//  Lcd.print(DuncanCount);
 
  pinMode(OPEN_DOOR_PIN,INPUT_PULLUP);
  
  
  //pinMode(LED_PIN,OUTPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  digitalWrite(BUZZER_PIN,LOW);
  delay(100);
  digitalWrite(BUZZER_PIN,HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN,LOW);

  DoorMotor.initalize(STEP_IN1,STEP_IN2,STEP_IN3,STEP_IN4,OPEN_LIMIT_PIN,CLOSE_LIMIT_PIN);
  activateDoor(0);
}

void loop() {
  unsigned long tagID=0UL; 
  static unsigned long timeClosed=0UL;
  static boolean isDoorClosed = false;

  String data;
if(Serial.available()>0){
  data=Serial.readString();
  switch(data[0]){
    case 'c':
      DoorMotor.setStepperDirection(1);
      DoorMotor.stepsToTake(60);
      DoorMotor.disableStepper();
      Serial.println("C");Serial.println(DoorMotor.getStepperPosition());
    break;
    case 'w':
      DoorMotor.setStepperDirection(0);
      DoorMotor.stepsToTake(60);
      DoorMotor.disableStepper();
      Serial.println("W");Serial.println(DoorMotor.getStepperPosition());
    break;
    case 'o':
      DoorMotor.moveToCwLimit();
    break;
    case 's':
      DoorMotor.moveToCcwLimit();
    break;
  }
}

  if(digitalRead(OPEN_DOOR_PIN)==LOW){
    timeClosed -= ClosedDelay;
  }
  
  if(isDoorClosed==false){ // only check RFID if door is open
    tagID=checkRFID();
  }
  if(tagID > 0UL && isDoorClosed==false){  // Start counter
    if(isValidTag(tagID)==true){
      timeClosed=millis();
      isDoorClosed=activateDoor(1); //close door
    }
  }
  if(isDoorClosed==true){
    //checkBuzzer(millis()-timeClosed);
    if((millis()-timeClosed) > ClosedDelay){
      isDoorClosed=activateDoor(0); // open door
      clearRFIDBuffer();
    }
  }

}
