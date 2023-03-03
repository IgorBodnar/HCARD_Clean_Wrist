#include "StepMotor.h"

#define JOYSTICK_VRX_PIN A0
#define JOYSTICK_VRY_PIN A1
#define JOYSTICK_SW_PIN A2

#define JOYSTICK_X_REST_VALUE 510
#define JOYSTICK_Y_REST_VALUE 504

#define TS_JOYSTICK 200
#define TS_BUTTON 20
#define CLICK_DETECTION_TIMEOUT 3000

int joystickValueX = 0;
int joystickValueY = 0;

int previousJoystickButtonState = 0;
int currentJoystickButtonState = 0;

bool clickDetectionInvoked = false;
uint32_t clickDetectionInvokedTimestamp = 0;

int clickCounter = 0;

uint32_t joystickTimer = 0;
uint32_t buttonTimer = 0;

StepMotor RightMotor;
StepMotor LeftMotor;

void joystickLog(){
  Serial.print("x = ");
  Serial.print(joystickValueX);
  Serial.print(", y = ");
  Serial.print(joystickValueY);
  Serial.print(", switch_state = ");
  Serial.println(currentJoystickButtonState);
}

void clickDetectionHandler(){
  if (clickDetectionInvoked){
    uint32_t timeElapsed = millis() - clickDetectionInvokedTimestamp;
    
    if (timeElapsed <= CLICK_DETECTION_TIMEOUT){
      if (currentJoystickButtonState == 1 && previousJoystickButtonState == 0){
        clickCounter++;
        Serial.println(clickCounter); 
      }
    }
    else {
      switch(clickCounter){
        case 1:
          Serial.println("Single Click");
          break;
          
        case 2:
          Serial.println("Double Click");
          break;
          
        case 3:
          Serial.println("Triple Click");
          break;

        default:
          Serial.println("Click pattern not recognised");
          break;
      }

      clickCounter = 0;
      clickDetectionInvoked = false;
    }
  }
}

ISR(TCA0_CMP1_vect){
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm;
  if (joystickValueY != JOYSTICK_Y_REST_VALUE || joystickValueX != JOYSTICK_X_REST_VALUE){
    RightMotor.spin();
    LeftMotor.spin();
  }
}

void setup() {
  // setting up timer interruptions
  noInterrupts(); //disable all interrupts
  TCA0.SINGLE.PER = 999;
  TCA0.SINGLE.CMP1 = 999;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm; //16 prescaler, 1M.
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm;

  interrupts();  //enable interrupts.
    
  Serial.begin(9600);
  LeftMotor.init(2, 3, 4, 5, 0);
  RightMotor.init(6, 7, 8, 9, 4);
  RightMotor.setDirection(false);
  RightMotor.setStepPeriod(2);
  LeftMotor.setDirection(true);
  LeftMotor.setStepPeriod(2);

}

void loop() {
  // joystick potentiometer sampling
  if (millis() - joystickTimer >= TS_JOYSTICK){
    joystickTimer = millis();
    
    joystickValueX = analogRead(JOYSTICK_VRX_PIN);
    joystickValueY = analogRead(JOYSTICK_VRY_PIN);
  }

  // joystick button sampling
  if (millis() - buttonTimer >= TS_BUTTON){
    buttonTimer = millis();
    
    previousJoystickButtonState = currentJoystickButtonState;
    currentJoystickButtonState = digitalRead(JOYSTICK_SW_PIN);

    if (currentJoystickButtonState == 0 && previousJoystickButtonState == 1 && !clickDetectionInvoked){
      clickDetectionInvoked = true;
      clickDetectionInvokedTimestamp = millis();
    }

    clickDetectionHandler();
  }

  // convert joystick data to motor speeds and directions
  if (joystickValueY == JOYSTICK_Y_REST_VALUE){
    int converted_speed = joystickValueX - JOYSTICK_X_REST_VALUE;
    if (converted_speed > 0){
      RightMotor.setDirection(false);
      LeftMotor.setDirection(false);
    }
    else
    {
      RightMotor.setDirection(true);
      LeftMotor.setDirection(true);
    }
    long step_period = 52-int(abs(converted_speed/10));
    RightMotor.setStepPeriod(1000*step_period);
    LeftMotor.setStepPeriod(1000*step_period);
  }
  else if (joystickValueX == JOYSTICK_X_REST_VALUE){
    int converted_speed = joystickValueY - JOYSTICK_Y_REST_VALUE;
    if (converted_speed > 0){
      RightMotor.setDirection(true);
      LeftMotor.setDirection(false);
    }
    else
    {
      RightMotor.setDirection(false);
      LeftMotor.setDirection(true);
    }
    long step_period = 52-int(abs(converted_speed/10));
    RightMotor.setStepPeriod(1000*step_period);
    LeftMotor.setStepPeriod(1000*step_period);
  }
  else {
    int converted_speed_y = joystickValueY - JOYSTICK_Y_REST_VALUE;
    int converted_speed_x = joystickValueX - JOYSTICK_X_REST_VALUE;
    long step_period = 52-int(abs(converted_speed_y/10));
    long step_period_offset = int(abs(converted_speed_x/10));
    long step_period_right = step_period;
    long step_period_left = step_period;
    if (converted_speed_y > 0){
      if (converted_speed_x < 0){
        step_period_left += step_period_offset;
        RightMotor.setDirection(true);
        LeftMotor.setDirection(true);
      }
      else{
        step_period_right += step_period_offset;
        RightMotor.setDirection(false);
        LeftMotor.setDirection(false);
      }
    }
    else
    {
      if (converted_speed_x < 0){
        step_period_right += step_period_offset;
        RightMotor.setDirection(true);
        LeftMotor.setDirection(true);
      }
      else{
        step_period_left += step_period_offset;
        RightMotor.setDirection(false);
        LeftMotor.setDirection(false);
      }
    }
    RightMotor.setStepPeriod(1000*step_period_right);
    LeftMotor.setStepPeriod(1000*step_period_left);
  }

  

  //joystickLog();
}
