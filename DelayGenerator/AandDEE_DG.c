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

//4 TTL on single port (AandDEE.v0.0.1)
#define TTL_DDR  DDRD 
#define TTL_PORT PORTD
#define TTL_PIN  PIND
//Upper Left TTL
#define TTL_UL PORTD6
//Upper Right TTL
#define TTL_UR PORTD5
//Bottom Left TTL 
#define TTL_BL PORTD7
//Bottom Right TTL
#define TTL_BR PORTD4

#endif

//TTL camera  (out)
#define TTL_camera  TTL_UL
//TTL matrixi  (out)
#define TTL_matrixi TTL_UR

//LED camera  (out)
#define LED_camera  LED_UL
//LED matrixi  (out)
#define LED_matrixi LED_UR
//LED wait next reset  (out)
#define LED_wait    LED_BR

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

//wait_TTL(TTL_PIV,LED_PIV);
inline void wait_TTL(char ttl,char led)
{
///wait for start trigger on TTL
  //LED ON (i.e. !TTL)
  LED_PORT|=_BV(led);
  //wait
  loop_until_bit_is_set(TTL_PIN,ttl); //wait for TTL synchronization up
  //LED OFF (i.e. !TTL)
  LED_PORT&=~_BV(led);
}

/*
//wait_TTL_(TTL_PIV,LED_PIV);
#define wait_TTL_(ttl,led) \
{ \
  LED_PORT|=_BV(led); \
  loop_until_bit_is_set(TTL_PIN,ttl); \
  LED_PORT&=~_BV(led); \
}
*/

//wait_TTL_(PIV);
#define wait_TTL_(pin) \
{ \
  LED_PORT|=_BV(LED_##pin); \
  loop_until_bit_is_set(TTL_PIN,TTL_##pin); \
  LED_PORT&=~_BV(LED_##pin); \
}

//!
/**
 * record a single image (with both TTL and LED)
**/
inline void record_image(const int delayUp,const int delay1)
{
    //ON
    ///TTL
    TTL_PORT|=_BV(TTL_camera);//TTL on camera
    ///LED
    LED_PORT|=_BV(LED_camera);//LED on camera
    ///LED and TTL matrixi
    LED_PORT|=_BV(LED_matrixi);//LED on matrixi
    TTL_PORT|=_BV(TTL_matrixi);//TTL on matrixi
    //delay (i.e. TTL up time)
    _delay_ms(delayUp);//delay0
    //OFF
    TTL_PORT&=~_BV(TTL_camera);//TTL off camera
    TTL_PORT&=~_BV(TTL_matrixi);//TTL off matrixi
    ///delay (i.e. LED exposure time)
    _delay_ms(delay1);
    LED_PORT&=~_BV(LED_camera);//LED off camera
    LED_PORT&=~_BV(LED_matrixi);//LED off matrixi
}//record_image

//
int main(void)
{
//initialisation
///TTL
//  TTL_DDR&=~_BV(TTL_UL);//TTL input
  TTL_DDR|=_BV(TTL_camera)|_BV(TTL_matrixi);//TTL output: sync for camera (FlowMaster, ImagerIntense, Phantom, ...)
///LED
  LED_DDR|=_BV(LED_camera)|_BV(LED_matrixi)|_BV(LED_wait);//|_BV(LED_UL);//LED output: TTL_UL and wait for reset

/** /
//mapping
  int ttl[4]={TTL_UL,TTL_UR,TTL_BL,TTL_BR};
  int led[6]={LED_UL,LED_UR,LED_BL,LED_BR};//,LED_AL,LED_AR};
//test
  LED_DDR=255;//all LED output
  testAllLED(2,500,led);
  testLEDmap(2,500,led,ttl);
  testAllLED(1,1000,led);
/**/

//initialisation
  LED_PORT=0;//all LED off

//internal delay generator
///TTL
int period=250;//period=delay0+delay1+delay2
int delayUp=10;//delay0
int delayDown=period-delayUp;
///LED
int exposure=100;
int delay1=exposure-delayUp;//exposure=delay0+delay1
int delay2=delayDown-delay1;//delayDown=delay1+delay2

//sequence
  int i;
  for(i=0;i<100;++i) {record_image(delayUp,delay1);_delay_ms(delay2);}
  LED_PORT|=_BV(LED_wait);//LED on wait
//loop
  while(1){}//infinite loop
  return (0);
}

