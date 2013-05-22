#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>

//Arduino UNO mapping (AtMega328p)
//anolog in
//C0-5=0-5
//digital in/out
//Di=i [0-7]
//B0=8, B1=9, B2=10, B3=11, B4=12, B5=13 [! B6&7 crystal !!]

//TTL shield
//TTL: 4-7  <=> D4-7
//LED: 8-11 <=> B0-3

#ifdef MCU_atmega328p
 
//4 LED on single port (AandDEE.v0.0.1)
#define LED_DDR  DDRD
#define LED_PORT PORTD
//Upper Left LED
#define LED_UL  PORTD2
//Upper Right LED
#define LED_UR  PORTD1
//Bottom Left LED
#define LED_BL  PORTD0
//Bottom Right LED
#define LED_BR  PORTD3

//3 bit wheel system
#define BIT_DDR  DDRB
#define BIT_PORT PORTB
#define BIT_PIN  PINB
//Upper Left LED
#define BIT_0  PORTB5
#define BIT_1  PORTB4
#define BIT_2  PORTB3

#endif

//"Error 1 __builtin_avr_delay_cycles expects an integer constant. "

//! delay_ms instead of _delay_ms as internal implementation has changed in AVR-Libc 1.7.1 (not precise as old _delay_ms was not, ? new one)
void delay_ms(uint16_t count)
{
  while(count--) _delay_ms(1);
}

//! delay_us instead of _delay_us as internal implementation has changed in AVR-Libc 1.7.1 (not precise as _delay_us was not, ? new one)
void delay_us(uint16_t count)
{
  while(count--) _delay_us(1);
}

//! 
/**
 * 
 * 
 * 
 * 
**/
void testAllLED(int repeat,int delay,int *led/*[6]*/)
{
  int i,r;
  //repeat loop
  for(r=0;r<repeat;++r)
  {
    //LED on
    for(i=0;i<4;++i) LED_PORT|=_BV(led[i]);
    //wait a while
    delay_ms(delay);
    //LED off
    for(i=0;i<4;++i) LED_PORT&=~_BV(led[i]);
    //wait a while
    delay_ms(delay);
  }//for loop test
}//testAllLED

//! 
/**
 * 
 * 
 * 
 * 
**/
void testLEDmap(int repeat,int delay,int *led/*[4]*/)
{
  int i,r;
  //repeat loop
  for(r=0;r<repeat;++r)
  {
    //digital LED
    for(i=0;i<4;++i)
    {
      //ON
      LED_PORT|=_BV(led[i]);//LED on
      delay_ms(delay);
      //OFF
      LED_PORT&=~_BV(led[i]);//LED off
      delay_ms(delay);
    }//digital loop
    delay_ms(delay);
  }//repeat loop
}//testLEDmap

inline void set_LED_for_bit(int bit,int led)
{
  if(bit_is_set(BIT_PIN,bit)) LED_PORT|=_BV(led); else LED_PORT&=~_BV(led);
}

inline void set_LEDs(unsigned char value)
{
  if(value&0b00000001) {LED_PORT|=_BV(LED_UL);} else {LED_PORT&=~_BV(LED_UL);}
  if(value&0b00000010) {LED_PORT|=_BV(LED_UR);} else {LED_PORT&=~_BV(LED_UR);}
  if(value&0b00000100) {LED_PORT|=_BV(LED_BL);} else {LED_PORT&=~_BV(LED_BL);}
}

inline void test_LED_map(void)
{
  unsigned char i=0;
  for(i=0;i<7;++i) {set_LEDs(i);delay_ms(1234);}
}

inline uint8_t get_value(uint8_t wheel)
{
  uint8_t value=0;
  if( !(wheel&_BV(BIT_0)) ) value|=0b00000001;
  if( !(wheel&_BV(BIT_1)) ) value|=0b00000010;
  if( !(wheel&_BV(BIT_2)) ) value|=0b00000100;
  return value;
}

//
int main(void)
{
//initialisation
///BIT
  BIT_DDR&=~_BV(BIT_0);//as input (connected to ground or free: seems strange/random behaviour)
  BIT_DDR&=~_BV(BIT_1);//as input
  BIT_DDR&=~_BV(BIT_2);//as input
///LED
  LED_DDR|=_BV(LED_BL)|_BV(LED_BR)|_BV(LED_UL)|_BV(LED_UR);//LED output

/**/
//mapping
  int led[4]={LED_UL,LED_UR,LED_BL,LED_BR};
//test
  LED_DDR=255;//all LED output
  testAllLED(2,500,led);
//  testLEDmap(2,500,led);
//  testAllLED(1,1000,led);
/**/

//3 bit wheel system test program
  LED_PORT=0;//all LED off

  testAllLED(1,250,led);
  test_LED_map();
  testAllLED(1,250,led);

  uint8_t wheel;
  uint8_t value;
  int delay;
//loop
  while(1)
  {
  wheel=_SFR_BYTE(BIT_PIN);
  value=get_value(wheel);
  set_LEDs(value);
  delay=345*(int)(value+1);
  delay_ms(delay);
  testAllLED(1,250,led);
/*
    set_LED_for_bit(BIT_0,LED_UL);
    set_LED_for_bit(BIT_1,LED_UR);
    set_LED_for_bit(BIT_2,LED_BL);
/*
code
 i bin led
 0 000 111
 1 100 011
 2 010 101
 3 110 001
 4 001 110
 5 101 010
 6 011 100
*/
  }//infinite loop
  return (0);
}

