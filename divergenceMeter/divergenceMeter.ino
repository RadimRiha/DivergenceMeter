#define N7_0 13
#define N7_1 6
#define N7_2 5
#define N7_3 4
#define N7_4 3
#define N7_5 2
#define N7_6 1
#define N7_7 0
#define N7_8 15
#define N7_9 14
#define N7_PL 7
#define N7_PR 12
#define N6_0 17
#define N6_1 10
#define N6_2 9
#define N6_3 8
#define N6_4 23
#define N6_5 22
#define N6_6 21
#define N6_7 20
#define N6_8 19
#define N6_9 18
#define N6_PL 11
#define N6_PR 16
#define N5_0 37
#define N5_1 30
#define N5_2 29
#define N5_3 28
#define N5_4 27
#define N5_5 26
#define N5_6 25
#define N5_7 24
#define N5_8 39
#define N5_9 38
#define N5_PL 31
#define N5_PR 36
#define N4_0 41
#define N4_1 34
#define N4_2 33
#define N4_3 32
#define N4_4 47
#define N4_5 46
#define N4_6 45
#define N4_7 44
#define N4_8 43
#define N4_9 42
#define N4_PL 35
#define N4_PR 40
#define N3_0 61
#define N3_1 54
#define N3_2 53
#define N3_3 52
#define N3_4 51
#define N3_5 50
#define N3_6 49
#define N3_7 48
#define N3_8 63
#define N3_9 62
#define N3_PL 55
#define N3_PR 60
#define N2_0 65
#define N2_1 58
#define N2_2 57
#define N2_3 56
#define N2_4 71
#define N2_5 70
#define N2_6 69
#define N2_7 68
#define N2_8 67
#define N2_9 66
#define N2_PL 59
#define N2_PR 64
#define N1_0 85
#define N1_1 78
#define N1_2 77
#define N1_3 76
#define N1_4 75
#define N1_5 74
#define N1_6 73
#define N1_7 72
#define N1_8 87
#define N1_9 86
#define N1_PL 79
#define N1_PR 84
#define N0_0 89
#define N0_1 82
#define N0_2 81
#define N0_3 80
#define N0_4 95
#define N0_5 94
#define N0_6 93
#define N0_7 92
#define N0_8 91
#define N0_9 90
#define N0_PL 83
#define N0_PR 88

#define NUM_OF_NIXIES 8
#define NUM_OF_REGISTERS 12
#define BRIGHTNESS_LEVELS 50

uint8_t displayContent[NUM_OF_NIXIES] = {11, 10, 6, 4, 3, 2, 1, 0};  //10 - PL, 11 - PR, 12 - empty
uint8_t displayBrightness[NUM_OF_NIXIES] = {50, 50, 50, 50, 50, 50, 50, 50};

volatile uint8_t shiftState[NUM_OF_REGISTERS] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
volatile uint8_t displayState = 0b11111111;    //8 PWM channels for each nixie (N7...N0)
volatile uint8_t PWMcounter = 0;

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
  PORTB |= 0b00001000;
  uint8_t prevDisplayState = displayState;
  for(uint8_t n = 0; n < NUM_OF_NIXIES; n++) {
    if(PWMcounter < displayBrightness[n]) displayState |= (1 << n);
    else displayState &= ~(1 << n);
  }
  if(displayState != prevDisplayState || PWMcounter == 0) {
    generateShiftState();
    shiftEverything();
  }
  PWMcounter++;
  if(PWMcounter >= BRIGHTNESS_LEVELS) PWMcounter = 0;
  PORTB &= ~0b00001000;
}

void generateShiftState() {
  for(uint8_t i = 0; i < NUM_OF_REGISTERS; i++) {   //clear shiftState
    shiftState[i] = 0x00;
  }
  for(uint8_t n = 0; n < NUM_OF_NIXIES; n++) {      //set digit of each nixie
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

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 5000; //about 60Hz at 50 levels
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 1
  TCCR1B |= (1 << CS10);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

void loop() {
  for(uint8_t i = 0; i <= 12; i++) {
    displayContent[0] = i;
    displayContent[1] = i;
    displayContent[2] = i;
    displayContent[3] = i;
    displayContent[4] = i;
    displayContent[5] = i;
    displayContent[6] = i;
    displayContent[7] = i;
    delay(1000);
  }
}
