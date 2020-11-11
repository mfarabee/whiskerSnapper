#include "motors.h"
////////////////////////////
//  Stepper_28BYJ_48 CLASS METHODS
///////////////////////////

  void Stepper_28BYJ_48::stepsToTake(int stepCount){
    int counter=0,limitPin,limit;
    if(CcwLimitPin == 0){
      StepperPtr->step(stepCount*StepDirection);
      CurrentStepPosition=(CurrentStepPosition+(stepCount*StepDirection))%STEPS_PER_OUTPUT_REVOLUTION;
    }else{    
      if(StepDirection>0){ //Clockwise
        limitPin=CwLimitPin;
      }else{
        limitPin=CcwLimitPin;
      }
      limit=digitalRead(limitPin);
      while(limit==HIGH && counter < stepCount){
        //Serial.print("Pin: ");Serial.println(limit);
        StepperPtr->step(1*StepDirection);
        limit=digitalRead(limitPin);
        ++counter;
      }
      CurrentStepPosition=(CurrentStepPosition+(counter*StepDirection))%STEPS_PER_OUTPUT_REVOLUTION;
    }
  }
  
  void Stepper_28BYJ_48::findZero(){ 
    int zeroSwitch,counter;
    if(CcwLimitPin == 0){
      // counter is used to stop the zeroing function if the stepper travels more than 1 rotation
      counter = -20; // give an arbitrary buffer, so that is will go at least one revolution
      zeroSwitch=digitalRead(CwLimitPin);
      while(zeroSwitch==LOW && counter < STEPS_PER_OUTPUT_REVOLUTION){
        StepperPtr->step(1*StepDirection);
        zeroSwitch=digitalRead(CwLimitPin);
        ++counter;
      }
      while(zeroSwitch==HIGH && counter < STEPS_PER_OUTPUT_REVOLUTION){
        StepperPtr->step(1*StepDirection);
        zeroSwitch=digitalRead(CwLimitPin);
        ++counter;
      }
    }else{
      zeroSwitch=digitalRead(CcwLimitPin);
      while(zeroSwitch==HIGH){
        StepperPtr->step(-1);
        zeroSwitch=digitalRead(CcwLimitPin);
        ++counter;
      }
    }
    CurrentStepPosition=0;
  }
  
  void  Stepper_28BYJ_48::setStepperSpeed(int stepperSpeed){ // stepperSpeed =  0%-100%
    stepperSpeed=map(stepperSpeed,0,100,5,MaxSpeed_28BYJ);
    StepperPtr->setSpeed(stepperSpeed);
  }
    
  void Stepper_28BYJ_48::initalize(int in1, int in2, int in3, int in4,int zero){
    In1Pin=in1;In2Pin=in2;In3Pin=in3;In4Pin=in4;CwLimitPin=zero;
    pinMode(CwLimitPin,INPUT_PULLUP);
    // IN1=34 IN2=36 IN3=38 IN4=40
    //Stepper Stepper1(STEPS_PER_MOTOR_REVOLUTION,TIDE_STEP_IN1,TIDE_STEP_IN3,TIDE_STEP_IN2,TIDE_STEP_IN4);
    StepperPtr = new Stepper (STEPS_PER_MOTOR_REVOLUTION,In1Pin,In3Pin,In2Pin,In4Pin);
    setStepperSpeed(50);
    findZero();
  }
  
    void Stepper_28BYJ_48::initalize(int in1, int in2, int in3, int in4,int cw_limit,int ccw_limit){
    In1Pin=in1;In2Pin=in2;In3Pin=in3;In4Pin=in4;CwLimitPin=cw_limit;CcwLimitPin=ccw_limit;
    pinMode(CwLimitPin,INPUT_PULLUP);
    pinMode(CcwLimitPin,INPUT_PULLUP);
    // IN1=34 IN2=36 IN3=38 IN4=40
    //Stepper Stepper1(STEPS_PER_MOTOR_REVOLUTION,TIDE_STEP_IN1,TIDE_STEP_IN3,TIDE_STEP_IN2,TIDE_STEP_IN4);
    StepperPtr = new Stepper (STEPS_PER_MOTOR_REVOLUTION,In1Pin,In3Pin,In2Pin,In4Pin);
    setStepperSpeed(50);
    
  }

  void Stepper_28BYJ_48::moveToCwLimit(){
    int limit;
    setStepperDirection(1);
    limit=digitalRead(CwLimitPin);
      while(limit==HIGH){
        stepsToTake(1);
        limit=digitalRead(CwLimitPin);
      }
      disableStepper();
  }
  void Stepper_28BYJ_48::moveToCcwLimit(){
    int limit;
    setStepperDirection(0);
    limit=digitalRead(CcwLimitPin);
      while(limit==HIGH){
        stepsToTake(1);
        limit=digitalRead(CcwLimitPin);
      }
      disableStepper();
  }
  void Stepper_28BYJ_48::setZero(){
    findZero();
  }
  
  int Stepper_28BYJ_48::getStepperPosition(){
    return(CurrentStepPosition);
  }

  void Stepper_28BYJ_48::setStepperDirection(int sdir){
    if(sdir >0){ // clockwise
      StepDirection =1;
    }else{
      StepDirection= -1;
    }
  }
  
  void Stepper_28BYJ_48::disableStepper(){
    digitalWrite(In1Pin,LOW);digitalWrite(In2Pin,LOW);digitalWrite(In3Pin,LOW);digitalWrite(In4Pin,LOW);
  }
    
  void Stepper_28BYJ_48::positionAngle(int angle){
    int newPosition,stepsToTake;
    newPosition=map(angle,0,360,0,STEPS_PER_OUTPUT_REVOLUTION);
    if(StepDirection==1){
      stepsToTake=(STEPS_PER_OUTPUT_REVOLUTION-(CurrentStepPosition - newPosition))%STEPS_PER_OUTPUT_REVOLUTION;
    }else{
      stepsToTake=(-STEPS_PER_OUTPUT_REVOLUTION+(newPosition- CurrentStepPosition))%STEPS_PER_OUTPUT_REVOLUTION;
    }
    StepperPtr->step(stepsToTake);
    CurrentStepPosition=(CurrentStepPosition+stepsToTake)%STEPS_PER_OUTPUT_REVOLUTION;
  }

