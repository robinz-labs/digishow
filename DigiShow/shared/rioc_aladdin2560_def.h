/*
    Copyright 2021 Robin Zhang & Labs

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

// pins on arduino board

#define PIN_UNO_A0   14
#define PIN_UNO_A1   15
#define PIN_UNO_A2   16
#define PIN_UNO_A3   17
#define PIN_UNO_A4   18
#define PIN_UNO_A5   19
#define PIN_UNO_A6   20
#define PIN_UNO_A7   21

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

// pins on aladdin 2560 board (2019)

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

const int PIN_UD_ADC[4] = {
    PIN_UD_ADC0, PIN_UD_ADC1, PIN_UD_ADC2, PIN_UD_VREF
};

#endif // RIOC_ALADDIN2560_DEF_H
