/*
Pulse and Sample

coil_scan

wombatpi.net

Modified 09-May-2024


Notes:  
The Sample window is sensitive to all digital and analog IO that occurs during.
Thus, it is enclosed in a no-interrupts block .

*/

#include <arduino.h>
//#include "pwm.h"
#include "FspTimer.h"
#include "wombat_analog.h"



FspTimer pulse_timer ;
int8_t tindex ;
uint8_t timer_type ;


// number of samples to take in sample window
//
volatile int sampleCounter = 0;
volatile uint16_t sampleArray[SAMPLE_COUNT_MAX];


#define PULSE_10uSEC (10)
#define PULSE_15uSEC (15)
#define PULSE_20uSEC (20)
#define PULSE_30uSEC (30)
#define PULSE_40uSEC (40)
#define PULSE_50uSEC (50)
#define PULSE_60uSEC (60)
#define PULSE_70uSEC (70)
#define PULSE_80uSEC (80)
#define PULSE_90uSEC (90)
#define PULSE_93uSEC (93)
#define PULSE_100uSEC (100)
#define PULSE_120uSEC (120)
#define PULSE_200uSEC (200)
#define PULSE_264uSEC (264)
#define PULSE_300uSEC (300)
#define PULSE_540uSEC (540)






#define USEC_DELAYB asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");



#define USEC_DELAYA asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");\
asm("NOP");



volatile bool TIME_TO_PULSE = true;

void timerADCReadInterrupt(timer_callback_args_t __attribute((unused)) *p_args)
{
  int sampleCounter;      
  noInterrupts();  

  if (TIME_TO_PULSE)
  {    
    pulse_timer.stop();
    pulse_timer.set_period_us(PULSE_WIDTH) ; 
    pulse_timer.start(); 
    *PFS_P104PFS_BY = 0x05;      // Pulse on D3 

    // the next interrupt will be the end of the pulse
    //
    TIME_TO_PULSE = false;

    interrupts();
    return;
  }

  // end of pulse
  //
  *PFS_P104PFS_BY = 0x04;      // Pulse off D3 

  // time to sample
  //
  // scan delay
  // The 3 options are:
  // no delay
  // 1uSec delay
  // 2uSec delay
  //
  // The 3 sets of samples at 3uSec intervals are then interleaved to give 1uSec resolution
  //
  // 
  //*PFS_P107PFS_BY = 0x05;         // digitalWrite(monitorPinD7, HIGH)  
 if(scanDelay > 0)
  {
    // 1 uSec Delay
    //   
    USEC_DELAYA;   

    if(scanDelay > 1)
    {
      // another 1 uSec Delay
      //   
      USEC_DELAYB;         
    }      
  }

  // Sampling
  //    
  for(sampleCounter = 0 ; sampleCounter < SAMPLE_COUNT_MAX; sampleCounter++)
  {
    *ADC140_ADCSR |= (0x01 << 15);  // Next ADC conversion = write to register c. 300nS
    while((*ADC140_ADCSR &= (0x01 << 15) ) != 0x0000);  // if things not setup right, endless loop
    
    sampleArray[sampleCounter] = *ADC140_ADDR01;         
  }  

  pulse_timer.stop();  

  pulse_timer.set_period_us(PULSE_FREQUENCY_PERIOD) ;  
  
  pulse_timer.start();

  // the next interrupt will be the start of a pulse
  //
  TIME_TO_PULSE = true;
  
  interrupts();     
 
  sampleReady = true;   
}




// coil pulse and sample using timer, pin D3
//
bool setupPulse()
{
  timer_type = GPT_TIMER;
  tindex = FspTimer::get_available_timer(timer_type);
  
  pinMode(D3, OUTPUT);

  if (tindex < 0){
    tindex = FspTimer::get_available_timer(timer_type, true);
  }
  if (tindex < 0){
    return false;
  }

  FspTimer::force_use_of_pwm_reserved_timer();

  // Start the Pulse and sampling timer at 100Hz,
  // Actual pulse_frequency and pulse_width are set in the interrupt
  //
  //  
  if(!pulse_timer.begin(TIMER_MODE_PERIODIC, timer_type, tindex, 100, 0.0f, timerADCReadInterrupt)){
    return false;
  }  

  if (!pulse_timer.setup_overflow_irq()){
    return false;
  }

  if (!pulse_timer.open()){
    return false;
  }

  if (!pulse_timer.start()){
    return false;
  }
  return true;
}




void setupSample() {

  pinMode(analogPin, INPUT);  
  analogReference(AR_EXTERNAL);
  
  analogReadResolution(14);        // This code reads ADC result directly, so this setup not needed
  *DAC12_DAADSCR |= (0x01 << 7);     // set the D/A A/D sync bit 
  
  uint16_t value = analogRead(analogPin);  // Do a read to get everthing set-up
  // Update registers - do NOT use analogRead() after this

  *ADC140_ADCER = 0x06;              // 14 bit mode (already set), clear ACE bit 5
  
  //-------------------------------------------------------------
 
  *ADC140_ADCSR |= (0x01 << 15);     // Start a conversion
  
  adc_val_16 = 8;      
}

