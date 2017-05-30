#pragma once


// Max7219 Pins
#define MAX_DATA_IN 48
#define MAX_CLK 49
#define MAX_LOAD 46
#define MAX_INTENSITY 15 // 0-15

// PIC*595 Pins
#define TPIC_SERIAL_IN 47 // PL2(T5)
#define TPIC_SRCK 44 // CLOCK PL5(OC5C)
#define TPIC_RCK 42 // LATCH PL7
#define TPIC_CLR 45 // PL4 (OC5B)

// Rotary Encoders

#define ENC_PULSE 4

// Vspeed
#define ENC_E1A 18
#define ENC_E1B A0
#define ENC_E1PS A4
#define ENC_E1PL A5

// Altitude
#define ENC_E2A 19
#define ENC_E2B A1
#define ENC_E2PS A6
#define ENC_E2PL A7

// HDG
#define ENC_E3A 20
#define ENC_E3B A2
#define ENC_E3PS 4
#define ENC_E3PL 7 

// SPEED
#define ENC_E4A 21
#define ENC_E4B A3
#define ENC_E4PS 5
#define ENC_E4PL 6

// SWITCHES

#define SW_LOC A8
#define SW_HDGTRK A9
#define SW_ATHR A10
#define SW_EXPED A11
#define SW_AP1 A12
#define SW_AP2 A13
#define SW_METALT A14
#define SW_APPR A15
#define SW_SPDMACH 33



// LEDs

#define LED_EXPED 43
#define LED_AP2 41
#define LED_APPR 40
#define LED_LOC 39
#define LED_ATHR 38
#define LED_AP1 36

#define LED_MODE_MACH 37
#define LED_MODE_SPEED 35

#define LED_MODE_HDG 34
#define LED_MODE_TRK 32

#define LED_MANAGED_ALT 29
#define LED_MANAGED_HDG 30
#define LED_MANAGED_SPD 31
