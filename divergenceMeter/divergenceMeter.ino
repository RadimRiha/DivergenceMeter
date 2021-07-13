#define N0_0
#define N0_1
#define N0_2
#define N0_3
#define N0_4
#define N0_5
#define N0_6
#define N0_7
#define N0_8
#define N0_9
#define N0_PL
#define N0_PR
#define N1_0
#define N1_1
#define N1_2
#define N1_3
#define N1_4
#define N1_5
#define N1_6
#define N1_7
#define N1_8
#define N1_9
#define N1_PL
#define N1_PR
#define N2_0
#define N2_1
#define N2_2
#define N2_3
#define N2_4
#define N2_5
#define N2_6
#define N2_7
#define N2_8
#define N2_9
#define N2_PL
#define N2_PR
#define N3_0
#define N3_1
#define N3_2
#define N3_3
#define N3_4
#define N3_5
#define N3_6
#define N3_7
#define N3_8
#define N3_9
#define N3_PL
#define N3_PR
#define N4_0
#define N4_1
#define N4_2
#define N4_3
#define N4_4
#define N4_5
#define N4_6
#define N4_7
#define N4_8
#define N4_9
#define N4_PL
#define N4_PR
#define N5_0
#define N5_1
#define N5_2
#define N5_3
#define N5_4
#define N5_5
#define N5_6
#define N5_7
#define N5_8
#define N5_9
#define N5_PL
#define N5_PR
#define N6_0
#define N6_1
#define N6_2
#define N6_3
#define N6_4
#define N6_5
#define N6_6
#define N6_7
#define N6_8
#define N6_9
#define N6_PL
#define N6_PR
#define N7_0
#define N7_1
#define N7_2
#define N7_3
#define N7_4
#define N7_5
#define N7_6
#define N7_7
#define N7_8
#define N7_9
#define N7_PL
#define N7_PR

uint8_t displayContent[8] = {12, 12, 12, 12, 12, 12, 12, 12};  //10 - PL, 11 - PR, 12 - empty

uint8_t displayBrightness[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

void assembleShift() {
  uint16_t data = 0;  //only 12 bits used
  for(uint8_t i = 0; i < 8; i++) {
    if(pinMap[i][displayContent[i]] != 0xFF) data |= (1UL << pinMap[i][displayContent[i]]);
  }
}

void setup() {

}
void loop() {

}
