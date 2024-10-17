#ifndef _FULL_SCAN_H
#define _FULL_SCAN_H

#define FULL_SCAN_SIZE (150)

// if using double d coil, then response is negative (decrease in signal) so we mulitiply scan by -1
//
#define DOUBLE_D (false)

double fullScanData[FULL_SCAN_SIZE];
double compareScanData[FULL_SCAN_SIZE];  // for target compared to reference scan


#endif
