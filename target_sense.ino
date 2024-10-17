/*
--------------------------------------------------------------------
wombatpi.net

target_sense.ino

coil_scan project


Target ID and discrimination

Modified 14-Apr-2024

--------------------------------------------------------------------
*/

#include "target_sense.h"
 

#define MIN_FLOAT (-32000.0)
#define MAX_FLOAT (32000.0)

// normalise the array to values 1.0 to 0.0
// Quickest, used on curves where the min and max are known.
//
void normalise(double arr[], int sz, int minIndex, int maxIndex)
{
  double min = arr[minIndex];
  double max = arr[maxIndex];
  double range;  

  // normalise
  //
  range = max-min;
  if(range == 0)
  {
    range = 1.0; // catch divide-by-zero
  }
  for(int i = 0 ; i < sz; i++)
  {
    arr[i] -= min;
    arr[i] /= range;    
  }
}


// normalise the array to values 1.0 to 0.0
// used on unsorted curves where the min and max are not obvious and unknown
// i.e the Target-shape curve
//
void normalise(double arr[], int sz)
{
  double min = MAX_FLOAT;
  double max = MIN_FLOAT;
  double range;

  // find max and min
  //
  for(int i = 0 ; i < sz; i++)
  {
    if(min > arr[i])
    {
      min = arr[i];
    }
    if (max < arr[i])
    {
      max = arr[i];
    }
  }

  // normalise
  //
  range = max-min;
  if(range == 0)
  {
    range = 1.0; // catch divide-by-zero
  }
  for(int i = 0 ; i < sz; i++)
  {
    arr[i] -= min;
    arr[i] /= range;    
  }
}



// estimate the peak in Time using 4 points, evenly spaced samples,  3uSec apart
// ! must be 4 values in the array 
//
double findPeak(double sig_curve[], int first_frequency)
{
  double total;
  total = sig_curve[0] * first_frequency;
  first_frequency += 3;
  total += sig_curve[1] * first_frequency;
  first_frequency += 3;
  total += sig_curve[2] * first_frequency;
  first_frequency += 3;
  total += sig_curve[3] * first_frequency;

  total /= (sig_curve[0] + sig_curve[1] + sig_curve[2]  + sig_curve[3]);   // divide by sum of 'y's
  return(total);
}


void IS_TARGET_A(double r2, double r3)
{  
  if(r2 > 0.7)
  {
    TARGET_SENSE::targetID = OK_BIG;    
  }
  else if ((r2 -r3)  > 0.2)
  {
    TARGET_SENSE::targetID = OK_SMALL;
  }
  else
  {
    TARGET_SENSE::targetID = Fe;
  }
}



