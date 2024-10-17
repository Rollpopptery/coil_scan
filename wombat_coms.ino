
/*
WOMBAT PI Metal detector
wombatpi.net
Modified: 14-Apr-2024

Communication over the serial port 
Serial rate: 115200
Command end with Enter (Carriage return)

MS    = Scan  (0 to 149uSec)
MC    = Compare scan (do another scan and subtract first scan)
MN    = Normalise  , print out the Compare scan normalised.

MT    = continuous scan

*/

// include the stdlib, so atoi function works
//
#include <stdlib.h>

#include "wombat.h"

#define MAX_COMMAND (10)
#define END_CHAR (13)

char rxBuffer[MAX_COMMAND];
int rxCount = 0;


//--------------------------------------------------------------------------------------------
// Processing of all Serial / Wifi Commands
//
void checkCommands()
{
  char inChar = 0;
  bool commandIn = false;
  int tempValue = 0;

  // ensure quick return if no serial
  //
  if(Serial.available() <= 0)
  {
    return;
  } 
  
  
  while(Serial.available() > 0)
  {
    inChar = Serial.read();  
    rxBuffer[rxCount] = inChar;  

    // end of command character ?
    if(inChar == END_CHAR)
    {
      rxBuffer[rxCount] = 0;  // null terminated the buffer
      commandIn = true;
      break;
    }    
    else
    {
      rxCount++;
      if(rxCount >= MAX_COMMAND)
      {
        // avoid buffer overflow
        //
        rxCount = 0;
      }       
    }    
  }
  // read everything else and ignore
  //
  while(Serial.available() > 0)
  {
    Serial.read();
  } 
  rxCount = 0;

  // do we have a command ?
  //
  if(commandIn == true)
  {    
    // process the command
    // 
    if(rxBuffer[0] == 'M')
    {      
      // Set Mode
      //
      switch(rxBuffer[1])
      {        
        case('S'):
        {      
          mode = S;
        }
        break;   
        case('C'):
        {      
          mode = C;
        }
        break;  
        case('N'):
        {      
          mode = N;
        }
        break;  
        case('a'):
        {      
          mode = a;
        }
        break;
        case('T'):
        {
          mode = T;
        }
        break;


        default:
        {
          // do nothing
        }
        break;        
      }
    }    
  }  
}
