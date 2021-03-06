/*	Author: lab
 *  Partner(s) Name: Charles Hong
 *	Lab Section: 022
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: toggle sound, scale up or down
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * Demo Link: https://drive.google.com/file/d/1BN7N_olHBI8ExP8CiggrSdNz8DVoYbRA/view?usp=sharing
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#define A (PINA&0x03)
#define A2 (PINA&0x04)
#define C4 261.63
#define D4 293.66
#define E4 329.63
#define F4 349.23
#define G4 392.00
#define A4 440.00
#define B4 493.88
#define C5 523.25
unsigned char i = 0;
enum Power_State {p_start, fr, np, nr, fp} p_state;
enum State {start, release, plus, minus} state;
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
double freq[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
//                  C       D       E       F       G       A       B       C5

void Power_Tick() {
    switch(p_state) {
        case p_start: 
        p_state = fr;
        PWM_on();
        break;
        case fr:
        if(A2) {
            p_state = np;
        }
        else p_state = fr;
        break;

        case np:
        if(A2) p_state = np;
        else p_state = nr;
        break;

        case nr:
        if(A2) {
            p_state = fp;
        }
        else p_state = nr;
        break;

        case fp:
        if(A2) p_state = fp;
        else p_state = fr;
        break;
    }
    switch(p_state) {
        case start: break;
        case np:
        case nr:
        set_PWM(freq[i]);
        break;

        case fp:
        case fr:
        set_PWM(0);
        break;
    }
}

void Tick() {
    switch(state) {
        case start:
        i=0;
        state = release;
        break;
        case release:
        if(A==0x01) {
            state = plus;
            if(i<7) i++;
        } else if(A==0x02) {
            state = minus;
            if(i>0) i--;
        } else state = release;
        break;
        case plus:
        if(A==0x01) state = plus;
        else state = release;
        break;
        case minus:
        if(A==0x02) state = minus;
        else state = release;
    }
}
int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x40; PORTB = 0x00;
    PORTA = PINA;
    p_state = p_start;
    state = start;
    i=0;
    while (1) {
        Power_Tick();
        Tick();
    }
    return 1;
}
