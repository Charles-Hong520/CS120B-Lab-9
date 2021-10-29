/*	Author: lab
 *  Partner(s) Name: Charles Hong
 *	Lab Section: 022
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: toggle sound, scale up or down
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
// #define A (PINA&0x03)
// #define A2 (PINA&0x04)
#define C 261.63
#define D 293.66
#define E 329.63
#define F 349.23
#define G 392.00
#define A 440.00
#define B 493.88
#define C5 523.25
enum Power_State {p_start, fr, np, nr, fp} p_state;
enum State {start, release, plus, minus} state;

volatile unsigned char TimerFlag = 0; //TimerISR sets it to 1, programmer sets it to 0
unsigned long _avr_timer_M = 1; //start count from here, down to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0; //current internal count of 1ms ticks

const unsigned long periodGCD = 65;
unsigned char timeElapsed = 0;
unsigned char i = 0;


void TimerOn() {
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1  = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

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

// unsigned char note_period[] = {4,3,1,6,2,4,4,6};
// double notes[] = {C5, B4, A4, G4, F4, E4, D4, C4};

// unsigned char note_period[] = {2,2,2,4,2,1,1,1,1,2,6,2,6};
// double notes[] = {C5,0,B4,0,E4,A4,0,A4,0,F4,0,E4,0};

// double notes[] = {A4,0,A4,0,A4,0,G4,0,G4,0,E4,0,E4,0,G4};
// unsigned char note_period[] = {3,1,1,1,1,1,2,2,2,2,2,2,2,2,8};


// void Power_Tick() {
//     switch(p_state) {
//         case p_start: 
//         p_state = fr;
//         PWM_on();
//         break;
//         case fr:
//         if(A2) {
//             p_state = np;
//         }
//         else p_state = fr;
//         break;

//         case np:
//         if(A2) p_state = np;
//         else p_state = nr;
//         break;

//         case nr:
//         if(A2) {
//             p_state = fp;
//         }
//         else p_state = nr;
//         break;

//         case fp:
//         if(A2) p_state = fp;
//         else p_state = fr;
//         break;
//     }
//     switch(p_state) {
//         case start: break;
//         case np:
//         case nr:
//         set_PWM(freq[i]);
//         break;

//         case fp:
//         case fr:
//         set_PWM(0);
//         break;
//     }
// }

void Tick() {

}
int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x4F; PORTB = 0x00;
    PORTA = PINA;
    TimerSet(periodGCD);
    TimerOn();
    PWM_on();
const unsigned char s = 1;
double notes[] = {0,E,0,B,0, B,0, A,0, B,0, A,0, G,0, A,0, A,0, G,0, E,0, G};
unsigned char note_period[] = {s,9,s,9,s,4,s,4,s,9,s,4,s,4,s,9,s,4,s,4,s,4,s,4}; //24
unsigned char size = 24;
    while (1) {
        // Power_Tick();
        set_PWM(notes[i]);
        if(timeElapsed >= note_period[i]) {
            timeElapsed = 0;
            i++;
            if(i>=24) i = 0;
        }
        timeElapsed++;
        PORTB = 1<<(i%5);
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
