#ifndef MOTORS_H
#define MOTORS_H

#include "Arduino.h"
#include <Stepper.h>  // Stepper 28BYJ-48

#define STEPS_PER_MOTOR_REVOLUTION 32
#define STEPS_PER_OUTPUT_REVOLUTION 2048
#define MaxSpeed_28BYJ 800

class Stepper_28BYJ_48{
      int StepDirection = 1;
      int CurrentStepPosition=0;
      int In1Pin,In2Pin,In3Pin,In4Pin,CwLimitPin=0,CcwLimitPin=0;
      Stepper *StepperPtr;
      
      void findZero();
      
    public:
      void initalize(int,int,int,int,int);
      void initalize(int,int,int,int,int,int);
      void setZero();
       int getStepperPosition();
      void setStepperSpeed(int);
      void setStepperDirection(int);
      void disableStepper();
      void positionAngle(int);
      void stepsToTake(int);
      void moveToCwLimit();
      void moveToCcwLimit();
};

#endif
