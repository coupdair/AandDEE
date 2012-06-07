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
 
//6 LED on single port
#define LED_DDR  DDRB
#define LED_PORT PORTB
//Upper Left LED
#define LED_UL  PORTB5
//Upper Right LED
#define LED_UR  PORTB2
//Bottom Left LED
#define LED_BL  PORTB4
//Bottom Right LED
#define LED_BR  PORTB1
//Analog Left LED
#define LED_AL  PORTB3
//Analog Right LED
#define LED_AR  PORTB0

//4 TTL on single port
#define TTL_DDR  DDRD 
#define TTL_PORT PORTD
#define TTL_PIN  PIND
//Upper Left TTL
#define TTL_UL PORTD6
//Upper Right TTL
#define TTL_UR PORTD4
//Bottom Left TTL 
#define TTL_BL PORTD5
//Bottom Right TTL
#define TTL_BR PORTD7

//2 analog on single port
//! \todo analog read for both pin.

#endif

//TTL burst (in)
#define TTL_burst TTL_BL
//TTL PIV   (in)
#define TTL_PIV   TTL_UL
//TTL camera  (out)
#define TTL_camera  TTL_BR
//TTL matrixi (out)
#define TTL_matrixi TTL_UR

//LED burst (in)
#define LED_burst LED_BL
//LED PIV   (in)
#define LED_PIV   LED_UL
//LED camera  (out)
#define LED_camera  LED_BR
//LED matrixi (out)
#define LED_matrixi LED_UR

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
 * record a single image (with matrixi on if BURST defined)
**/
inline void record_image(const int delayUp,const int delay1)
{
    //ON
    ///TTL
    TTL_PORT|=_BV(TTL_camera);//TTL on camera
#ifdef BURST
    TTL_PORT|=_BV(TTL_matrixi);//TTL on matrixi
#endif //BURST
    ///LED
    LED_PORT|=_BV(LED_camera);//LED on camera
#ifdef BURST
    LED_PORT|=_BV(LED_matrixi);//LED on matrixi
#endif //not BURST
    //delay (i.e. TTL up time)
    _delay_ms(delayUp);//delay0
    //wait external PIV trigger
    loop_until_bit_is_clear(TTL_PIN,TTL_PIV); //wait for PIV synchronization down
    //OFF
    TTL_PORT&=~_BV(TTL_camera);//TTL off camera
#ifdef BURST
    TTL_PORT&=~_BV(TTL_matrixi);//TTL off matrixi
#endif //BURST
    //delay (i.e. LED exposure time)
    _delay_ms(delay1);
    LED_PORT&=~_BV(LED_camera);//LED on camera
#ifdef BURST
    LED_PORT&=~_BV(LED_matrixi);//LED on matrixi
#endif //BURST
}//record_image

//!
/**
 * increment matrixi
**/
inline void do_matrixi(const int delayUp,const int delay1)
{
    //ON
    ///TTL
    TTL_PORT|=_BV(TTL_matrixi);//TTL on matrixi
    ///LED
    LED_PORT|=_BV(LED_matrixi);//LED on matrixi
    //delay (i.e. TTL up time)
    _delay_ms(delayUp);//delay0
    //wait external PIV trigger
    loop_until_bit_is_clear(TTL_PIN,TTL_PIV); //wait for PIV synchronization down
    //OFF
    TTL_PORT&=~_BV(TTL_matrixi);//TTL off matrixi
    //delay (i.e. LED exposure time)
    _delay_ms(delay1);
    LED_PORT&=~_BV(LED_matrixi);//LED on matrixi
}//record_image

//
int main(void)
{
//initialisation
///TTL
  TTL_DDR&=~_BV(TTL_UL);//TTL input: PIV 4Hz in
  TTL_DDR&=~_BV(TTL_BL);//TTL input: burst envelop 0.1 Hz in
  TTL_DDR|=_BV(TTL_UR);//TTL output: matrixi 4Hz out
  TTL_DDR|=_BV(TTL_BR);//TTL output: camera trigging burst out
///LED
  LED_DDR|=_BV(LED_BL)|_BV(LED_BR)|_BV(LED_UL)|_BV(LED_UR);//LED output

/** /
//mapping
  int ttl[4]={TTL_UL,TTL_BL,TTL_UR,TTL_BR};
  int led[6]={LED_UL,LED_BL,LED_UR,LED_BR,LED_AL,LED_AR};
//test
  testAllLED(2,500,led);
  testLEDmap(2,500,led,ttl);
  testAllLED(1,1000,led);
/**/

//internal delay generator
///TTL
int delayUp=10;//delay0 MAstPIV:10ms
///LED
int exposure=150;//MAstPIV:<4Hz (i.e. 250 ms)
int delay1=exposure-delayUp;//exposure=delay0+delay1

///wait for start trigger on TTL burst
//wait_TTL(TTL_burst,LED_burst);
wait_TTL_(burst);

//loop
  int i;
  while(1)
  {
#ifdef BURST
    ///wait for start trigger on TTL burst
    wait_TTL_(burst);
    ///delay between valves and measurement
    //set by burst system (i.e. ArduinoMega1280)
    ///wait for start trigger on TTL PIV
    wait_TTL_(PIV);
    ///record a first selection of images
    record_image(delayUp,delay1);
    for(i=1;i<10;++i) {wait_TTL_(PIV);record_image(delayUp,delay1);}
    ///skip recording of images
    for(i=0;i<10;++i) {wait_TTL_(PIV);do_matrixi(delayUp,delay1);}
    ///record a first selection of images
    for(i=0;i<10;++i) {wait_TTL_(PIV);record_image(delayUp,delay1);}
#else //!BURST == CONTINUOUS
    wait_TTL_(PIV);
    record_image(delayUp,delay1);
#endif //!BURST

  }//infinite loop
  return (0);
}

