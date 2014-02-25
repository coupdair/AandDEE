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
#define TTL_UL PORTD6
//Upper Right TTL
#define TTL_UR PORTD5
//Bottom Left TTL 
#define TTL_BL PORTD7
//Bottom Right TTL
#define TTL_BR PORTD4

//2 analog on single port
//Analog INput C0-5
//AIN: A0-5 <=> C0-5
#define AIN 5
//threshold 5V = 10 bit = 1024
#define THRESHOLD 465

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

//ADC
unsigned int result;
unsigned int compare;

/*! \brief Initializes ADC Peripheral
     Configuration Registers */
void initialize(void)
{
//  DDRB    |= 0b00010001;        			// Set PORTB.0  as output
//  PORTB   |= 0b00010001;        			// Initialize PORTB.0 with logic "one"
  ADCSRA   = 0b10000100;// Enable ADC with Clock prescaled by 16 ; If Clock speed is 8MHz, then ADC clock = 8MHz/16 = 500kHz
  DIDR0    = 0b00111111;// Disable Digital Input on all ADC Channel to reduce power consumption
  //bc obase=2 5 101
  ADMUX    = 0b11000000;// 0b11000101 Disable Left-Adjust and select Internal 1.1V reference and ADC Channel 5 '0101' as input (0 '0000') p265
  compare  = (unsigned int)THRESHOLD;// (465 -> 0.5V Equivalent Counts for 1.1 V ADC Reference)
}//initialize

/*! \brief ADC Conversion Routine
 *  in single ended mode */
void convert(void)
{
  ADCSRA  |= (1<<ADSC);			// Start ADC Conversion
  while((ADCSRA & (1<<ADIF)) != 0x10);	// Wait till conversion is complete
  result   = ADC;                       // Read the ADC Result
  ADCSRA  |= (1 << ADIF);		// Clear ADC Conversion Interrupt Flag
//  if(result <= compare)                 // Compare the converted result with 0.5 V
  if(result > compare)                 // Compare the converted result with 0.5 V
    LED_PORT|=_BV(LED_BR);//LED on
  else
    LED_PORT&=~_BV(LED_BR);//LED off
}//convert

//
int main(void)
{
//initialisation
///TTL
/**/
  TTL_DDR|=_BV(TTL_UL);//TTL output
  TTL_DDR|=_BV(TTL_UR);//TTL output
  TTL_DDR|=_BV(TTL_BR);//TTL output
//  TTL_DDR|=_BV(TTL_BL);//TTL output
  TTL_DDR&=~_BV(TTL_BL);//TTL input
/*or for ArduinoUNO* /
//  TTL_DDR=0b01111111;//BL for AandDEE.shield.v0.0.1 or BR for AandDEE.shield.v0.1.1
  TTL_DDR=0b11111111;//all output
/*but NOT*/
  //TTL_DDR=255;//TTL output BAD
  //TTL_DDR&=~_BV(TTL_BL);//TTL input BAD
///LED
  LED_DDR|=_BV(LED_BL)|_BV(LED_BR)|_BV(LED_UL)|_BV(LED_UR);//LED output

//mapping
  int ttl[4]={TTL_UL,TTL_UR,TTL_BR/*L*/,TTL_BR};
  int led[6]={LED_UL,LED_UR,LED_BL,LED_BR,LED_AL,LED_AR};

//ADC
#define ADC_ENABLE

#ifdef ADC_ENABLE
initialize();
#endif

//test
/**/
  testAllLED(2,500,led);
//  testLEDmap(2,500,led,ttl);
//  testAllLED(1,1000,led);
/**/

#ifdef ADC_ENABLE
_delay_ms(12000);//test: wait a while for ADC ready ?
#endif

//loop
  int delay=234;
LED_PORT|=_BV(LED_BL);//LED on
  while(1)
  {
LED_PORT|=_BV(LED_UL);//LED on
LED_PORT&=~_BV(LED_UR);//LED off
    //ADC test
#ifdef ADC_ENABLE
    convert();
#else
    // D  test
    if( bit_is_set(TTL_PIN,TTL_BL) )
      LED_PORT|=_BV(LED_BR);//LED on
    else
      LED_PORT&=~_BV(LED_BR);//LED off
#endif
LED_PORT&=~_BV(LED_UL);//LED off
LED_PORT|=_BV(LED_UR);//LED on
    //wait for next ADConvertion, so lighting LED could be seen.
    _delay_ms(delay);
  }//infinite loop
  return (0);
}

