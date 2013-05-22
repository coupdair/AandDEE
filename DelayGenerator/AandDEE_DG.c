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

//TTL trigger (in)
#define TTL_trigger TTL_UL
//TTL camera  (out)
#define TTL_camera  TTL_UR

//LED trigger (out)
#define LED_trigger LED_UL
//LED camera  (out)
#define LED_camera  LED_UR

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
    for(i=0;i<6;++i) LED_PORT|=_BV(led[i]);
    //wait a while
    delay_ms(delay);
    //LED off
    for(i=0;i<6;++i) LED_PORT&=~_BV(led[i]);
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
      delay_ms(delay);
      //OFF
      TTL_PORT&=~_BV(ttl[i]);//TTL down
      LED_PORT&=~_BV(led[i]);//LED off
      delay_ms(delay);
    }//digital loop
    delay_ms(delay);
    //analog LED
    for(i=4;i<6;++i)
    {
      //ON
      LED_PORT|=_BV(led[i]);//LED on
      delay_ms(delay);
      //OFF
      LED_PORT&=~_BV(led[i]);//LED off
      delay_ms(delay);
    }//analog loop
    delay_ms(delay);
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
    //delay (i.e. TTL up time)
    delay_ms(delayUp);//delay0
    //OFF
    TTL_PORT&=~_BV(TTL_camera);//TTL off camera
    //delay (i.e. LED exposure time)
    delay_ms(delay1);
    LED_PORT&=~_BV(LED_camera);//LED off camera
}//record_image

//
int main(void)
{
//initialisation
///TTL
  TTL_DDR&=~_BV(TTL_trigger);//TTL input
  TTL_DDR|=_BV(TTL_camera);//TTL output: sync for camera (FlowMaster, ImagerIntense, Phantom, ...)
///LED
  LED_DDR|=_BV(LED_camera)|_BV(LED_trigger);//|_BV(LED_UL)|_BV(LED_UR);//LED output: TTL_UL and wait for reset

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
const int period=1000;//period=delay0+delay1+delay2
const int delayUp=10;//delay0
const int delayDown=period-delayUp;
///LED
const int exposure=250;
const int delay1=exposure-delayUp;//exposure=delay0+delay1
const int delay2=delayDown-delay1;//delayDown=delay1+delay2

//sequence
  int i;
  for(i=0;i<10;++i)
  {
    wait_TTL_(trigger);
    delay_ms(123);
  }
  TTL_PORT|=_BV(TTL_camera);//LED on wait    
  LED_PORT|=_BV(LED_camera);//LED on wait
//loop
  while(1){}//infinite loop
  return (0);
}

