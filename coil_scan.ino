/*
----------------------------------------------------------------------------

WOMBAT PI Metal detector
COIL_SCAN
Arduino UNO R4 

wombatpi.net

Last Modified:  17-Oct-2024


----------------------------------------------------------------------------
*/

#define VERSION "21MAY24"

#include "wombat.h"
#include "wombat_analog.h"
#include "target_sense.h"
#include "full_scan.h"


#define BAUD_RATE (115200)

// Number of pulses to average
//
#define SAMPLE_BUFFER_LENGTH (50)




// raw analog samples to be averaged 
// Buffer SAMPLE_BUFFER_LENGTH long, contains a set of samples at different sample delays 'TIME_POINTS',
//
float samples[TIME_POINTS][SAMPLE_BUFFER_LENGTH];

// the last point / longest time on the discharge curve 
//
float sample_last [SAMPLE_BUFFER_LENGTH];

// For running averaging
//
double sums[TIME_POINTS] = {0.0};
double sum_last = 0.0;

double averages[TIME_POINTS] = {0.0};

double longAverages[TIME_POINTS] = {0.0};

double average_last = 0.0;
double mediumAverage_last = 0.0;
double longAverage_last = 0.0;

//double longAverageConduct[TIME_POINTS] = {0.0};
double averageConduct[TIME_POINTS] =  {0.0};

double normalised_average_curve[TIME_POINTS] = {0.0};
double normalised_curve[TIME_POINTS] = {0.0};

double signal_curve[TIME_POINTS] = {0.0};
double peak_signal_curve[TIME_POINTS] = {0.0};

#define R1 (0)
#define R2 (1)
#define R3 (2)
#define R4 (3)

double ratios[4]; 
int peakSampleCount = 0;  

#define MAX_TX_BUFFER (30)
char txBuffer[MAX_TX_BUFFER];




void setup() {  

  // the indexes are just every single raw sample from 0 to 50
  //
  for(int i = 0 ; i < TIME_POINTS; i++)
  {
    SAMPLE_INDEXES_SET1[i] = i;
  }

  Serial.begin(BAUD_RATE);

  setupSample();  
  setupPulse();
   

  for(int i = 0 ; i < SAMPLE_BUFFER_LENGTH; i++)
  {
    // do nothing
  }

  // Not used
  //
  if(WIFI_SERIAL_ENABLED)
  {
    Serial1.begin(BAUD_RATE);
  }
}


// The sample array contains samples every ~3uSec
// The sample delay is the index * 3uSec,
// eg 5 = 15 uSec
//
// What samples are we interested in , from the raw samples ?
//
int (*SAMPLE_INDEXES)[TIME_POINTS] = &SAMPLE_INDEXES_SET1;


#define ZERO (0)
#define ONE (1)
#define TWO (2)

// greater value means slower recovery, longer averaging
//
#define LONG_AVERAGE_FACTOR (5.0)


void loop() {  
  static int soundUpdateCount = 0;
  static int averageCount = 0;
  static double sum = 0.0;
  static int serialCheckCount = 0;   // check the serial port periodically
  static int printOutCount = 0;
  static int newAverageCounter;
  static double maxF;
  static int medianCounter = 0;
  static int loopCounter = 0;   

  int oldSample; 
  int index = 0;  
  double tempF ; 
  double signal;    
  
  if(sampleReady)
  {
    sampleReady = false;  
    
    // compensate_audio_for_pulse() ;  // compesnate for ~ 100Sec pulse
   
    // --running averages-- happens quckly and often (i.e at 500 Hz)
    // ! must be efficient !
    // replace the old value in the buffer with the new value
    // update the sum, re-calculate the average
    
    for(index = 0 ; index < TIME_POINTS; index++)
    {
      
      oldSample = samples[index][averageCount];           
      samples[index][averageCount] =( sampleArray[(*SAMPLE_INDEXES)[index]] );   // new raw sample from the set of samples, at the particular pulse width
      sums[index] -= oldSample;    // subtract old value from the sum
      sums[index] += samples[index][averageCount];  // add the new value to the sum

    }


    oldSample = sample_last[averageCount];
    tempF = 0;          
    sample_last[averageCount] = (sampleArray[SAMPLE_COUNT_MAX-1]);  
    sum_last -= oldSample;    // subtract old value from the sum
    sum_last += sample_last[averageCount];  // add the new value to the sum     

    // recalculate the running averages
    //    
    average_last = sum_last / SAMPLE_BUFFER_LENGTH;

    for(index = 0; index < TIME_POINTS; index++)
    {
      averages[index] = (sums[index] / SAMPLE_BUFFER_LENGTH);    
    }       

    printOutCount++;
    serialCheckCount++;
    soundUpdateCount++;
    
    averageCount++;   

    if(averageCount >= SAMPLE_BUFFER_LENGTH)
    {     
      // full set of samples complete
      //
      averageCount = 0;

      for(index = 0 ; index < TIME_POINTS; index++)
      {
        // re-calculate long averages
        //          
        tempF = (averages[index] -  longAverages[index]);
        tempF /= LONG_AVERAGE_FACTOR;          
        longAverages[index] += tempF;     

        // fast recovery, after the average has 'wound-up' due to being held on a target.
        // i.e The average follows the signal down quicker than it follows the target up.
        //
        if(tempF < 0)
        { 
           // add it again
           //
           longAverages[index] += tempF;    
        }
      }

      tempF = (average_last - longAverage_last);
      tempF /= LONG_AVERAGE_FACTOR;
      longAverage_last += tempF;     
    }
    
    
    // Do our serial communication here if its time,   
    //          
    if (serialCheckCount > 100)    
    {
      serialCheckCount = 0;
      // check serial commands
      //   
      checkCommands();      
    }

     
    // i.e If 600 Hz pulse rate, with 50-sample buffer, we will do this 600/50 = 12 times per second) 
    //
    else if (printOutCount  > SAMPLE_BUFFER_LENGTH)
    {      
      printOutCount = 0;
      
      // SCAN of Coil Pulse
      //
      if (mode == T)
      {        
        for(int i = 0; i < TIME_POINTS ; i++ )
        {          
          fullScanData[(i*3)+scanDelay] = (averages[i]);
        }
        scanDelay++;
        if(scanDelay > 2)
        {
          // print out the full scan
          //
          for(int i = 0 ; i < FULL_SCAN_SIZE; i++ )
          {
            Serial.print(fullScanData[i]);
            Serial.print(",");
          }
          scanDelay = 0; 
          Serial.println();         
        }
      }
      else if (mode == S)
      {        
        for(int i = 0; i < TIME_POINTS ; i++ )
        {          
          fullScanData[(i*3)+scanDelay] = (averages[i]);
        }
        scanDelay++;
        if(scanDelay > 2)
        {
          // print out the full scan
          //
          for(int i = 0 ; i < FULL_SCAN_SIZE; i++ )
          {
            Serial.println(fullScanData[i]);
          }
          scanDelay = 0;
          mode = s;
        }
      }
      // 'Compare' Scan
      //
      else if (mode == C)
      {   
        // we do this 3 times to get 3 scans at 3uSec intervals, to make up full scan at 1uSec interval
        //     
        for(int i = 0; i < TIME_POINTS ; i++ )
        {          
          compareScanData[(i*3)+scanDelay] = (averages[i]);
        }
        scanDelay++;
        if(scanDelay > 2)
        {
          // print out the full scan
          //
          for(int i = 0 ; i < FULL_SCAN_SIZE; i++ )
          {
            // print the difference
            //
            tempF = compareScanData[i] - fullScanData[i];
            if(DOUBLE_D)
            {
              tempF *= -1.0;   // make positive if double D coil
            }
            Serial.println(tempF);
          }
          scanDelay = 0;
          mode = s;  // stop
        }

      }
      // Print normalised scan  
      // (target scan - no-target scan)  normalised.
      //
      else if (mode == N)
      {        
        for(int i = 0 ; i < FULL_SCAN_SIZE; i++ )
          {
            // the difference
            //
            tempF = compareScanData[i] - fullScanData[i];
            if(DOUBLE_D)
            {
              tempF *= -1.0;   // make positive if double D coil
            }
            compareScanData[i] = tempF;
          }
        // normalise
        //
        normalise(compareScanData, FULL_SCAN_SIZE);

        // print out
        //
        for(int i = 0 ; i < FULL_SCAN_SIZE; i++ )
          {
            // print the difference
            //
            Serial.println(compareScanData[i]);
          }          
        mode = s; // stop
      }
    }     
  } 
}
