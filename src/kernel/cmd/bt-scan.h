//
// Created by 0xDC on 4/11/19.
//

#ifndef _KERNEL_BT_SCAN_H
#define _KERNEL_BT_SCAN_H

// All 16bit and 32bit UUID are really 128 UUIDs but compressed. To get the full form
// 128_form = (small_form << 96) + BLUETOOTH_BASE_UUID
// See Bluetooth Core Spec Vol.3 Part B Section 2.5



#define BB_VERBOSE // Determines wheter static strings, describing bluetooth characteristics, are stored in memory




// Bluetooth core supplement. Provides a bunch of UUID and other special numbers
// https://www.bluetooth.com/specifications
#endif //BLUE_BANDIT_BT_SCAN_H
