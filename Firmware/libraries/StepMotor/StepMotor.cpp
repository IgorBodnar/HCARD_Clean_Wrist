#include "StepMotor.h"

StepMotor::StepMotor() {}

void StepMotor::init(int sp1, int sp2, int sp3, int sp4, int addr) {
    stepPin1 = sp1;
    stepPin2 = sp2;
    stepPin3 = sp3;
    stepPin4 = sp4;
    eepromAddress = addr;

    pinMode(stepPin1, OUTPUT);
    pinMode(stepPin2, OUTPUT);
    pinMode(stepPin3, OUTPUT);
    pinMode(stepPin4, OUTPUT);

    readPositionEEPROM();
}

void StepMotor::setDirection(bool dir) {
    direction = dir;
}

void StepMotor::setStepPeriod(long period) {

    stepPeriod = (period >= MIN_STEP_PERIOD) ? period : MIN_STEP_PERIOD;
}

void StepMotor::step() {
    if(direction){
    switch(stepState){
      case 0:
        digitalWrite(stepPin1, HIGH);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, LOW);
        break;
      case 1:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, HIGH);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, LOW);
        break;
      case 2:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, HIGH);
        digitalWrite(stepPin4, LOW);
        break;
      case 3:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, HIGH);
        break;
    }
  }
  else {
    switch(stepState){
      case 0:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, HIGH);
        break;
      case 1:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, HIGH);
        digitalWrite(stepPin4, LOW);
        break;
      case 2:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, HIGH);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, LOW);
        break;
      case 3:
        digitalWrite(stepPin1, HIGH);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, LOW);
        break;
    }
  }

  stepState++;
  if (stepState > 3){stepState = 0;}
}

void StepMotor::goToPosition(int goal) {

  readPositionEEPROM();

  if (goal > position) {
    setDirection(false);
    while (position < goal) {
    if (micros() - currentMillisTime >= stepPeriod) {
      currentMillisTime = micros();
      step();
      position++;
    }
  }
  }
  else if (goal < position) {
    setDirection(true);
    while (position > goal) {
    if (micros() - currentMillisTime >= stepPeriod) {
      currentMillisTime = micros();
      step();
      position--;
    }
  }
  }

  writePositionEEPROM(position);

}

void StepMotor::writePositionEEPROM(int pos) {
  EEPROM.put(eepromAddress, pos);
}

void StepMotor::readPositionEEPROM() {
  EEPROM.get(eepromAddress, position);
}

int StepMotor::getPosition() {
  return position;
}

void StepMotor::zeroPosition() {
  position = 0;
  writePositionEEPROM(position);
}

void StepMotor::spin(){
  if (micros() - currentMillisTime >= stepPeriod) {
      currentMillisTime = micros();
      step();
      if (direction) {
        decrementPosition();
      }
      else {
        incrementPosition();
      }
    }
}

void StepMotor::incrementPosition() {
  position++;
  if (position == 2048){
    position = 0;
  }
  //writePositionEEPROM(position);
}

void StepMotor::decrementPosition() {
  position--;
  if (position == 0){
    position = 2048;
  }
  //writePositionEEPROM(position);
}