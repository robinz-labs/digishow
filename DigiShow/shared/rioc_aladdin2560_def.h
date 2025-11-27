/*
    Copyright 2021-2025 Robin Zhang & Labs

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

 */

#ifndef RIOC_ALADDIN2560_DEF_H
#define RIOC_ALADDIN2560_DEF_H

// pin types
#define PIN_TYPE_DI   (1 << 0) // X
#define PIN_TYPE_DO   (1 << 1) // Y
#define PIN_TYPE_DIO  (1 << 2) // D
#define PIN_TYPE_AI   (1 << 3) // A
#define PIN_TYPE_AO   (1 << 4) // DAC
#define PIN_TYPE_LED  (1 << 5) // LED
#define PIN_TYPE_SW   (1 << 6) // SW
#define PIN_TYPE_GPIO (1 << 7)

// pins on arduino board

#define PIN_UNO_A0   14
#define PIN_UNO_A1   15
#define PIN_UNO_A2   16
#define PIN_UNO_A3   17
#define PIN_UNO_A4   18
#define PIN_UNO_A5   19
#define PIN_UNO_A6   20
#define PIN_UNO_A7   21

const int PIN_UNO_ADC[8 ] = { PIN_UNO_A0, PIN_UNO_A1, PIN_UNO_A2, PIN_UNO_A3, PIN_UNO_A4, PIN_UNO_A5, PIN_UNO_A6, PIN_UNO_A7 };
const int PIN_UNO_DIO[12] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

#define PIN_MEGA_A0  54
#define PIN_MEGA_A1  55
#define PIN_MEGA_A2  56
#define PIN_MEGA_A3  57
#define PIN_MEGA_A4  58
#define PIN_MEGA_A5  59
#define PIN_MEGA_A6  60
#define PIN_MEGA_A7  61
#define PIN_MEGA_A8  62
#define PIN_MEGA_A9  63
#define PIN_MEGA_A10 64
#define PIN_MEGA_A11 65
#define PIN_MEGA_A12 66
#define PIN_MEGA_A13 67
#define PIN_MEGA_A14 68
#define PIN_MEGA_A15 69

const int PIN_MEGA_ADC[16] = { PIN_MEGA_A0, PIN_MEGA_A1, PIN_MEGA_A2,  PIN_MEGA_A3,  PIN_MEGA_A4,  PIN_MEGA_A5,  PIN_MEGA_A6,  PIN_MEGA_A7,
                               PIN_MEGA_A8, PIN_MEGA_A9, PIN_MEGA_A10, PIN_MEGA_A11, PIN_MEGA_A12, PIN_MEGA_A13, PIN_MEGA_A14, PIN_MEGA_A15 };
const int PIN_MEGA_DIO[46] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                               20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                               30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                               40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                               50, 51, 52, 53 };

#define PIN_MICRO_A0 18
#define PIN_MICRO_A1 19
#define PIN_MICRO_A2 20
#define PIN_MICRO_A3 21
#define PIN_MICRO_A4 22
#define PIN_MICRO_A5 23
#define PIN_MICRO_A6 24
#define PIN_MICRO_A7 25
#define PIN_MICRO_A8 26
#define PIN_MICRO_A9 27
#define PIN_MICRO_A10 28
#define PIN_MICRO_A11 29

const int PIN_MICRO_ADC[12] = { PIN_MICRO_A0, PIN_MICRO_A1, PIN_MICRO_A2, PIN_MICRO_A3, PIN_MICRO_A4, PIN_MICRO_A5, PIN_MICRO_A6, PIN_MICRO_A7, PIN_MICRO_A8, PIN_MICRO_A9, PIN_MICRO_A10, PIN_MICRO_A11 };
const int PIN_MICRO_DIO[15] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };


// pins on aladdin 2560 board (UD)

#define PIN_UD_RESET 41

#define PIN_UD_DI0   22
#define PIN_UD_DI1   23
#define PIN_UD_DI2   24
#define PIN_UD_DI3   25
#define PIN_UD_DI4   26
#define PIN_UD_DI5   27
#define PIN_UD_DI6   28
#define PIN_UD_DI7   29

#define PIN_UD_DI8   30
#define PIN_UD_DI9   31
#define PIN_UD_DI10  32
#define PIN_UD_DI11  33
#define PIN_UD_DI12  34
#define PIN_UD_DI13  35
#define PIN_UD_DI14  36
#define PIN_UD_DI15  37

#define PIN_UD_DO0   2
#define PIN_UD_DO1   3
#define PIN_UD_DO2   6
#define PIN_UD_DO3   7
#define PIN_UD_DO4   8
#define PIN_UD_DO5   11
#define PIN_UD_DO6   12
#define PIN_UD_DO7   13

#define PIN_UD_ADC0  54
#define PIN_UD_ADC1  55
#define PIN_UD_ADC2  56
#define PIN_UD_VREF  57

#define PORT_UD_232  1
#define PORT_UD_485  2

const int PIN_UD_DI[16] = {
    PIN_UD_DI0,  PIN_UD_DI1,  PIN_UD_DI2,  PIN_UD_DI3,
    PIN_UD_DI4,  PIN_UD_DI5,  PIN_UD_DI6,  PIN_UD_DI7,
    PIN_UD_DI8,  PIN_UD_DI9,  PIN_UD_DI10, PIN_UD_DI11,
    PIN_UD_DI12, PIN_UD_DI13, PIN_UD_DI14, PIN_UD_DI15
};

const int PIN_UD_DO[8] = {
    PIN_UD_DO0,  PIN_UD_DO1,  PIN_UD_DO2,  PIN_UD_DO3,
    PIN_UD_DO4,  PIN_UD_DO5,  PIN_UD_DO6,  PIN_UD_DO7
};

const int PIN_UD_ADC[3] = {
    PIN_UD_ADC0, PIN_UD_ADC1, PIN_UD_ADC2
};


// pins on aladdin micro board (UD1)

#define PIN_UD1_X0  PIN_MICRO_A5
#define PIN_UD1_X1  PIN_MICRO_A4
#define PIN_UD1_X2  PIN_MICRO_A3
#define PIN_UD1_X3  PIN_MICRO_A2
#define PIN_UD1_X4  4
#define PIN_UD1_X5  12
#define PIN_UD1_X6  8
#define PIN_UD1_X7  7

#define PIN_UD1_Y0  9
#define PIN_UD1_Y1  11
#define PIN_UD1_Y2  6
#define PIN_UD1_Y3  10
#define PIN_UD1_Y4  5
#define PIN_UD1_Y5  13

#define PIN_UD1_GPIO01 PIN_MICRO_A0
#define PIN_UD1_GPIO02 PIN_MICRO_A1
#define PIN_UD1_GPIO11 3  // I2C SCL
#define PIN_UD1_GPIO12 2  // I2C SDA
#define PIN_UD1_GPIO21 0  // UART RX
#define PIN_UD1_GPIO22 1  // UART TX

const int PIN_UD1_X  [8] = { PIN_UD1_X0, PIN_UD1_X1, PIN_UD1_X2, PIN_UD1_X3, PIN_UD1_X4, PIN_UD1_X5, PIN_UD1_X6, PIN_UD1_X7 };
const int PIN_UD1_Y  [6] = { PIN_UD1_Y0, PIN_UD1_Y1, PIN_UD1_Y2, PIN_UD1_Y3, PIN_UD1_Y4, PIN_UD1_Y5 };
const int PIN_UD1_ADC[2] = { PIN_UD1_GPIO01, PIN_UD1_GPIO02 };
const int PIN_UD1_DIO[4] = { PIN_UD1_GPIO21, PIN_UD1_GPIO22, PIN_UD1_GPIO12, PIN_UD1_GPIO11 };


// pins on aladdin mega board (UD2)

#define PIN_UD2_X0   22
#define PIN_UD2_X1   23
#define PIN_UD2_X2   24
#define PIN_UD2_X3   25
#define PIN_UD2_X4   26
#define PIN_UD2_X5   27
#define PIN_UD2_X6   28
#define PIN_UD2_X7   29

#define PIN_UD2_X8   30
#define PIN_UD2_X9   31
#define PIN_UD2_X10  32
#define PIN_UD2_X11  33
#define PIN_UD2_X12  34
#define PIN_UD2_X13  35
#define PIN_UD2_X14  36
#define PIN_UD2_X15  37

#define PIN_UD2_Y0   2
#define PIN_UD2_Y1   3
#define PIN_UD2_Y2   4
#define PIN_UD2_Y3   5
#define PIN_UD2_Y4   6
#define PIN_UD2_Y5   7
#define PIN_UD2_Y6   8
#define PIN_UD2_Y7   9
#define PIN_UD2_Y8   10
#define PIN_UD2_Y9   11
#define PIN_UD2_Y10  12
#define PIN_UD2_Y11  13

const int PIN_UD2_X  [16]= { PIN_UD2_X0, PIN_UD2_X1, PIN_UD2_X2, PIN_UD2_X3, PIN_UD2_X4, PIN_UD2_X5, PIN_UD2_X6, PIN_UD2_X7, PIN_UD2_X8, PIN_UD2_X9, PIN_UD2_X10, PIN_UD2_X11, PIN_UD2_X12, PIN_UD2_X13, PIN_UD2_X14, PIN_UD2_X15 };
const int PIN_UD2_Y  [12]= { PIN_UD2_Y0, PIN_UD2_Y1, PIN_UD2_Y2, PIN_UD2_Y3, PIN_UD2_Y4, PIN_UD2_Y5, PIN_UD2_Y6, PIN_UD2_Y7, PIN_UD2_Y8, PIN_UD2_Y9, PIN_UD2_Y10, PIN_UD2_Y11};
const int PIN_UD2_ADC[8] = { PIN_MEGA_A0, PIN_MEGA_A1, PIN_MEGA_A2, PIN_MEGA_A3, PIN_MEGA_A4, PIN_MEGA_A5, PIN_MEGA_A6, PIN_MEGA_A7 };
const int PIN_UD2_DIO[8] = { 40, 41, 42, 43, 44, 45, 46, 47 };


// pins on aladdin esp32 board (UD3)

#define PIN_UD3_X0   15
#define PIN_UD3_X1   16
#define PIN_UD3_X2   17
#define PIN_UD3_X3   18
#define PIN_UD3_X4   8
#define PIN_UD3_X5   38

#define PIN_UD3_Y0   21
#define PIN_UD3_Y1   14
#define PIN_UD3_Y2   13
#define PIN_UD3_Y3   12
#define PIN_UD3_Y4   10
#define PIN_UD3_Y5   9

#define PIN_UD3_ADC0 1
#define PIN_UD3_ADC1 11

#define PIN_UD3_DIO0 2
#define PIN_UD3_DIO1 4
#define PIN_UD3_DIO2 5

const int PIN_UD3_X  [6] = { PIN_UD3_X0, PIN_UD3_X1, PIN_UD3_X2, PIN_UD3_X3, PIN_UD3_X4, PIN_UD3_X5 };
const int PIN_UD3_Y  [6] = { PIN_UD3_Y0, PIN_UD3_Y1, PIN_UD3_Y2, PIN_UD3_Y3, PIN_UD3_Y4, PIN_UD3_Y5 };
const int PIN_UD3_ADC[2] = { PIN_UD3_ADC0, PIN_UD3_ADC1 };
const int PIN_UD3_DIO[3] = { PIN_UD3_DIO0, PIN_UD3_DIO1, PIN_UD3_DIO2 };


// pins on arduino plc model 1 (yushang)

#define PIN_PLC1_X0  25
#define PIN_PLC1_X1  26
#define PIN_PLC1_X2  27
#define PIN_PLC1_X3  28
#define PIN_PLC1_X4  29
#define PIN_PLC1_X5  39
#define PIN_PLC1_X6  30
#define PIN_PLC1_X7  31
#define PIN_PLC1_X10 32
#define PIN_PLC1_X11 33
#define PIN_PLC1_X12 34
#define PIN_PLC1_X13 35
#define PIN_PLC1_X14 36
#define PIN_PLC1_X15 37
#define PIN_PLC1_X16 40
#define PIN_PLC1_X17 41

#define PIN_PLC1_Y0  4
#define PIN_PLC1_Y1  5
#define PIN_PLC1_Y2  2
#define PIN_PLC1_Y3  3
#define PIN_PLC1_Y4  6
#define PIN_PLC1_Y5  7
#define PIN_PLC1_Y6  8
#define PIN_PLC1_Y7  9
#define PIN_PLC1_Y10 10
#define PIN_PLC1_Y11 11
#define PIN_PLC1_Y12 12
#define PIN_PLC1_Y13 47

#define PIN_PLC1_LED_RUN 24
#define PIN_PLC1_LED_ERR 23
#define PIN_PLC1_SWITCH  38

const int PIN_PLC1_X[16] = {
    PIN_PLC1_X0,  PIN_PLC1_X1,  PIN_PLC1_X2,  PIN_PLC1_X3,  PIN_PLC1_X4,  PIN_PLC1_X5,  PIN_PLC1_X6,  PIN_PLC1_X7,
    PIN_PLC1_X10, PIN_PLC1_X11, PIN_PLC1_X12, PIN_PLC1_X13, PIN_PLC1_X14, PIN_PLC1_X15, PIN_PLC1_X16, PIN_PLC1_X17
};

const int PIN_PLC1_Y[12] = {
    PIN_PLC1_Y0,  PIN_PLC1_Y1,  PIN_PLC1_Y2,  PIN_PLC1_Y3,  PIN_PLC1_Y4,  PIN_PLC1_Y5,  PIN_PLC1_Y6,  PIN_PLC1_Y7,
    PIN_PLC1_Y10, PIN_PLC1_Y11, PIN_PLC1_Y12, PIN_PLC1_Y13
};

const int PIN_PLC1_LED[2] = {
    PIN_PLC1_LED_RUN, PIN_PLC1_LED_ERR
};

const int PIN_PLC1_SW[1] = {
    PIN_PLC1_SWITCH
};


// pins on arduino plc model 2 (zanhor_a3)

#define PIN_PLC2_X0  2
#define PIN_PLC2_X1  3
#define PIN_PLC2_X2  18
#define PIN_PLC2_X3  19
#define PIN_PLC2_X4  26
#define PIN_PLC2_X5  27
#define PIN_PLC2_X6  28
#define PIN_PLC2_X7  29
#define PIN_PLC2_X10 30
#define PIN_PLC2_X11 31
#define PIN_PLC2_X12 32
#define PIN_PLC2_X13 33
#define PIN_PLC2_X14 34
#define PIN_PLC2_X15 35
#define PIN_PLC2_X16 36

#define PIN_PLC2_Y0  12
#define PIN_PLC2_Y1  46
#define PIN_PLC2_Y2  4
#define PIN_PLC2_Y3  5
#define PIN_PLC2_Y4  6
#define PIN_PLC2_Y5  7
#define PIN_PLC2_Y6  8
#define PIN_PLC2_Y7  9
#define PIN_PLC2_Y10 10
#define PIN_PLC2_Y11 11

#define PIN_PLC2_LED_RUN 13
#define PIN_PLC2_LED_ERR PIN_MEGA_A1
#define PIN_PLC2_SWITCH  37

const int PIN_PLC2_X[15] = {
    PIN_PLC2_X0,  PIN_PLC2_X1,  PIN_PLC2_X2,  PIN_PLC2_X3,  PIN_PLC2_X4,  PIN_PLC2_X5,  PIN_PLC2_X6,  PIN_PLC2_X7,
    PIN_PLC2_X10, PIN_PLC2_X11, PIN_PLC2_X12, PIN_PLC2_X13, PIN_PLC2_X14, PIN_PLC2_X15, PIN_PLC2_X16
};

const int PIN_PLC2_Y[10] = {
    PIN_PLC2_Y0,  PIN_PLC2_Y1,  PIN_PLC2_Y2,  PIN_PLC2_Y3,  PIN_PLC2_Y4,  PIN_PLC2_Y5,  PIN_PLC2_Y6,  PIN_PLC2_Y7,
    PIN_PLC2_Y10, PIN_PLC2_Y11
};

const int PIN_PLC2_LED[2] = {
    PIN_PLC2_LED_RUN, PIN_PLC2_LED_ERR
};

const int PIN_PLC2_SW[1] = {
    PIN_PLC2_SWITCH
};


#endif // RIOC_ALADDIN2560_DEF_H
