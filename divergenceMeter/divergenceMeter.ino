#include "hardwareDefinition.h"

#define BRIGHTNESS_LEVELS 50
const uint8_t expBrightness[BRIGHTNESS_LEVELS] = {1,1,1,1,1,1,1,2,2,2,2,2,3,3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,10,11,11,12,13,14,16,17,18,20,21,23,24,26,28,31,33,36,39,42,45,50};

uint8_t displayContent[NUM_OF_NIXIES] = {12, 12, 12, 12, 12, 12, 12, 12};   //10 - PL, 11 - PR, 12 - empty
uint8_t displayBrightness[NUM_OF_NIXIES] = {0, 0, 0, 0, 0, 0, 0, 0};        //0-BRIGHTNESS_LEVELS

volatile uint8_t shiftState[NUM_OF_REGISTERS] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
volatile uint8_t displayState = 0b11111111;    //8 PWM channels for each nixie (N7...N0)
volatile uint8_t PWMcounter = 0;

#define MENU_CLOCK_DISP 0
uint8_t menuState = MENU_CLOCK_DISP;

struct time {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
};
time internalTime = {0, 0, 0};
unsigned long lastMillisSecond = 0;

struct settings {
  bool leadingZero;         //06:00:00 vs 6:00:00
  bool format12_24;         //0 - 12, 1 - 24
  uint8_t brightness;       //0 - 100
  uint8_t cleaningInterval; //time [minutes] between cleaning cycles
};
settings clockSettings = {0, 1, 50, 1};

#define CLEANING_RATIO 300  //clean 1s for every 300s of run time
unsigned long lastCleanTimestamp = 0;

uint8_t pinMap[8][13] = {
  {N0_0, N0_1, N0_2, N0_3, N0_4, N0_5, N0_6, N0_7, N0_8, N0_9, N0_PL, N0_PR, 0xFF},
  {N1_0, N1_1, N1_2, N1_3, N1_4, N1_5, N1_6, N1_7, N1_8, N1_9, N1_PL, N1_PR, 0xFF},
  {N2_0, N2_1, N2_2, N2_3, N2_4, N2_5, N2_6, N2_7, N2_8, N2_9, N2_PL, N2_PR, 0xFF},
  {N3_0, N3_1, N3_2, N3_3, N3_4, N3_5, N3_6, N3_7, N3_8, N3_9, N3_PL, N3_PR, 0xFF},
  {N4_0, N4_1, N4_2, N4_3, N4_4, N4_5, N4_6, N4_7, N4_8, N4_9, N4_PL, N4_PR, 0xFF},
  {N5_0, N5_1, N5_2, N5_3, N5_4, N5_5, N5_6, N5_7, N5_8, N5_9, N5_PL, N5_PR, 0xFF},
  {N6_0, N6_1, N6_2, N6_3, N6_4, N6_5, N6_6, N6_7, N6_8, N6_9, N6_PL, N6_PR, 0xFF},
  {N7_0, N7_1, N7_2, N7_3, N7_4, N7_5, N7_6, N7_7, N7_8, N7_9, N7_PL, N7_PR, 0xFF}
};

ISR(TIMER1_COMPA_vect) {
  PORTB |= 0b00001000;  //DEBUG
  uint8_t prevDisplayState = displayState;
  for(uint8_t n = 0; n < NUM_OF_NIXIES; n++) {
    if(PWMcounter < expBrightness[displayBrightness[n]]) displayState |= (1 << n);
    else displayState &= ~(1 << n);
  }
  if(displayState != prevDisplayState || PWMcounter == 0) {
    generateShiftState();
    shiftEverything();
  }
  PWMcounter++;
  if(PWMcounter >= BRIGHTNESS_LEVELS) PWMcounter = 0;
  PORTB &= ~0b00001000; //DEBUG
}

void generateShiftState() {
  for(uint8_t i = 0; i < NUM_OF_REGISTERS; i++) shiftState[i] = 0x00;
  for(uint8_t n = 0; n < NUM_OF_NIXIES; n++) {    //set digit of each nixie
    if(displayState & (1 << n)){
      uint8_t bitNumber = pinMap[n][displayContent[n]];
      if (bitNumber != 0xFF) shiftState[bitNumber / 8] |= 1 << (bitNumber % 8);
    }
  }
}

void shiftEverything() {
  uint8_t currentRegisterState;
  for(uint8_t r = 0; r < NUM_OF_REGISTERS; r++) {   //each register
    currentRegisterState = shiftState[r];
    for(uint8_t b = 0; b < 8; b++) {                //8 bits per register
      //set data
      PORTB &= ~0b00000001;
      if(currentRegisterState & 0x01) PORTB |= 0b00000001;
      //shift one
      PORTB |= 0b00000010;
      PORTB &= ~0b00000010;
      currentRegisterState = currentRegisterState >> 1;
    }
  }
  //confirm
  PORTB |= 0b00000100;
  PORTB &= ~0b00000100;
}

void updateInternalTime() {
  unsigned long millisSecond = millis()/1000;
  if(millisSecond != lastMillisSecond) {
    internalTime.seconds++;
    if(internalTime.seconds >= 60) {
      internalTime.seconds = 0;
      internalTime.minutes++;
    }
    if(internalTime.minutes >= 60) {
      internalTime.minutes = 0;
      internalTime.hours++;
    }
    if(internalTime.hours >= 24) {
      internalTime.hours = 0;
    }
    lastMillisSecond = millisSecond;
  }
}

void setBrightness(uint16_t value) {
  uint8_t brightnessValue = value * BRIGHTNESS_LEVELS / 100;
  if(brightnessValue > BRIGHTNESS_LEVELS - 1) brightnessValue = BRIGHTNESS_LEVELS - 1;
  for(uint8_t i = 0; i < NUM_OF_NIXIES; i++) {
    displayBrightness[i] = brightnessValue;
  }
}

void cathodeCleaning() {
  if((millis() - lastCleanTimestamp) / 60000 >= clockSettings.cleaningInterval) {
    uint32_t cleaningTime = (uint32_t)clockSettings.cleaningInterval*60 * 1000 / CLEANING_RATIO;  //time to light up each digit [ms]
    setBrightness(100);
    for(uint8_t i = 0; i <= 11; i++) {
      for(uint8_t n = 0; n < NUM_OF_NIXIES; n++) displayContent[n] = i;
      delay(cleaningTime);
    }
    setBrightness(clockSettings.brightness);
    lastCleanTimestamp = millis();
  }
}

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  //PWM timer configuration
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 5000;             //about 60Hz at 50 levels
  TCCR1B |= (1 << WGM12);   //CTC
  TCCR1B |= (1 << CS10);    //Prescaler 1
  TIMSK1 |= (1 << OCIE1A);  //Output Compare Match A IE
  sei();

  //startup sequence
  setBrightness(100);
  for(uint8_t i = 0; i <= 12; i++) {
    for(uint8_t n = 0; n < NUM_OF_NIXIES; n++) displayContent[n] = i;
    delay(200);
  }
  
  //config defaults
  setBrightness(clockSettings.brightness);
}

void loop() {
  updateInternalTime();
  cathodeCleaning();
  switch (menuState) {
    case MENU_CLOCK_DISP:
      uint8_t compensatedHours = internalTime.hours;
      if(!clockSettings.format12_24) {
        compensatedHours = compensatedHours % 12;
      }
      displayContent[0] = internalTime.seconds%10;
      displayContent[1] = internalTime.seconds/10;
      displayContent[2] = 11;
      displayContent[3] = internalTime.minutes%10;
      displayContent[4] = internalTime.minutes/10;
      displayContent[5] = 11;
      displayContent[6] = compensatedHours%10;
      displayContent[7] = compensatedHours/10;
      if(displayContent[7] == 0 && !clockSettings.leadingZero) {
        displayContent[7] = 12;
      }
      break;
  }
}
