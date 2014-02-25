#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#define __DELAY_BACKWARD_COMPATIBLE__
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
 
//6 LED on single port
#define LED_DDR  DDRB
#define LED_PORT PORTB
//Upper Left LED
#define LED_UL  PORTB2
//Upper Right LED
#define LED_UR  PORTB1
//Bottom Left LED
#define LED_BL  PORTB0
//Bottom Right LED
#define LED_BR  PORTB3
//Analog Left LED
#define LED_AL  PORTB5
//Analog Right LED
#define LED_AR  PORTB4

//4 TTL on single port
#define TTL_DDR  DDRD 
#define TTL_PORT PORTD
#define TTL_PIN  PIND
//Upper Left TTL
#define TTL_UL PORTD4
//Upper Right TTL
#define TTL_UR PORTD6
//Bottom Left TTL 
#define TTL_BL PORTD7
//Bottom Right TTL
#define TTL_BR PORTD5

//2 analog on single port
//! \todo analog read for both pin.

#endif

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
    for(i=0;i<6;++i) LED_PORT|=_BV(led[i]);
    //wait a while
    _delay_ms(delay);
    //LED off
    for(i=0;i<6;++i) LED_PORT&=~_BV(led[i]);
    //wait a while
    _delay_ms(delay);
  }//for loop test
}//testAllLED

//! 
/**
 * 
 * 
 * 
 * 
**/
void testLEDmap(int repeat,int delay,int *led/*[6]*/,int *ttl/*[4]*/)
{
  int i,r;
  //repeat loop
  for(r=0;r<repeat;++r)
  {
    //digital LED
    for(i=0;i<4;++i)
    {
      //ON
      TTL_PORT|=_BV(ttl[i]);//TTL up
      LED_PORT|=_BV(led[i]);//LED on
      _delay_ms(delay);
      //OFF
      TTL_PORT&=~_BV(ttl[i]);//TTL down
      LED_PORT&=~_BV(led[i]);//LED off
      _delay_ms(delay);
    }//digital loop
    _delay_ms(delay);
    //analog LED
    for(i=4;i<6;++i)
    {
      //ON
      LED_PORT|=_BV(led[i]);//LED on
      _delay_ms(delay);
      //OFF
      LED_PORT&=~_BV(led[i]);//LED off
      _delay_ms(delay);
    }//analog loop
    _delay_ms(delay);
  }//repeat loop
}//testLEDmap

//
int main(void)
{
//initialisation
///TTL
/**/
  TTL_DDR|=_BV(TTL_UL);//TTL output
  TTL_DDR|=_BV(TTL_UR);//TTL output
  TTL_DDR|=_BV(TTL_BR);//TTL output
  TTL_DDR|=_BV(TTL_BL);//TTL output
/*or for ArduinoUNO* /
//  TTL_DDR=0b01111111;//BL for AandDEE.shield.v0.0.1 or BR for AandDEE.shield.v0.1.1
  TTL_DDR=0b11111111;//all output
/*but NOT*/
  //TTL_DDR=255;//TTL output BAD
  //TTL_DDR&=~_BV(TTL_BL);//TTL input BAD
///LED
  LED_DDR|=_BV(LED_BL)|_BV(LED_BR)|_BV(LED_UL)|_BV(LED_UR);//LED output

//mapping
  int ttl[4]={TTL_UL,TTL_UR,TTL_BL,TTL_BR};
  int led[6]={LED_UL,LED_UR,LED_BL,LED_BR,LED_AL,LED_AR};

//test
/** /
  testAllLED(2,500,led);
  testLEDmap(2,500,led,ttl);
  testAllLED(1,1000,led);
/**/

//loop
  int delay=1234;
  while(1)
  {
testAllLED(1,500,led);
testLEDmap(1,500,led,ttl);
/*
    //ON
    LED_PORT|=_BV(LED_UL);//LED on
    _delay_ms(delay);
    LED_PORT|=_BV(LED_BL);//LED on
    _delay_ms(delay);
    //OFF
    LED_PORT&=~_BV(LED_UL);//LED off
    _delay_ms(delay);
    LED_PORT&=~_BV(LED_BL);//LED off
    _delay_ms(delay);
*/
  }//infinite loop
  return (0);
}

