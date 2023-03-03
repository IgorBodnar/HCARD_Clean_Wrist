#ifndef STEPMOTOR
#define STEPMOTOR

#include <Arduino.h>
#include <EEPROM.h>

class StepMotor {
    public:
        StepMotor();
        void init(int sp1, int sp2, int sp3, int sp4, int addr);
        void step();
        void setDirection(bool dir);
        void setStepPeriod(long period);
        void goToPosition(int goal);
        int getPosition();
        void writePositionEEPROM(int pos);
        void readPositionEEPROM();
        void zeroPosition();
        void spin();

    private:
        void incrementPosition();
        void decrementPosition();
        int stepPin1;
        int stepPin2;
        int stepPin3;
        int stepPin4;
        bool direction = false;
        int stepState = 0;
        int position;
        uint32_t currentMillisTime = 0;
        long stepPeriod = 2;
        int eepromAddress;
        long MIN_STEP_PERIOD = 5000;
};

#endif