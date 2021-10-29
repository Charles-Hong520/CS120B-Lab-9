/*	Author: lab
 *  Partner(s) Name: Charles Hong
 *	Lab Section: 022
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: press for C D E
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#define A (PINA&0x07)
#define C4 261.63
#define D4 293.66
#define E4 329.63
#define F4 349.23
#define G4 392.00
#define A4 440.00
#define B4 493.88
#define C5 523.25

void set_PWM(double frequency) {
    static double current_frequency;
    if(frequency != current_frequency) {
        if(!frequency) {TCCR3B &= 0x08;}
        else {TCCR3B |= 0x03;}

        if(frequency < 0.954) {OCR3A = 0xFFFF;}
        else if(frequency > 31250) {OCR3A = 0x0000;}
        else {OCR3A = (short)(8000000/(128*frequency))-1;}
        TCNT3=0;
        current_frequency = frequency;
    }
}

void PWM_on() {
    TCCR3A = (1<<COM3A0);
    TCCR3B = (1<<WGM32) | (1<<CS31) | (1<<CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}
// double freq[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
//                    C       D       E       F       G       A       B       C5
int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x40; PORTB = 0x00;
    PORTA = PINA;
            PWM_on();

    while (1) {
        if(A == 0x01) {
            set_PWM(C4);
        } else if(A == 0x02) {
            set_PWM(D4);
        } else if(A == 0x04) {
            set_PWM(E4);
        } else {
            set_PWM(0);
        }
    }
    return 1;
}
