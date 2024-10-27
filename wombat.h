#ifndef _WOMBAT_H
#define _WOMBAT_H

// wombatpi.net

// settings in microseconds
//

// On the UNO R4, Serial out pins is 'Serial1'
// The USB serial is 'Serial'

bool WIFI_SERIAL_ENABLED = false;   // Serial1   not used
bool USB_SERIAL_ENABLED = true;     // Serial
bool LOCAL_AUDIO = true;            // sound output DAC

// communications mode (serial port data modes) described in wombat_coms
//

volatile enum {a, S, C, N, T, s, U} mode = a;

volatile enum {DEFAULT} targetMode = DEFAULT;



#endif
