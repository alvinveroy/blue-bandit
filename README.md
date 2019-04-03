# Blue Bandit
Battery powered mobile bluetooth pen-tester. Designed to conveniently hangout, say in your pocket, and autonomously pen-test bluetooth 4 & 5 devices in your vicinity. With the rise of IoT in common households, it will be interesting to see which products are doing their due diligence in protecting their consumers from security issues. This will strictly be "white-hat" i.e. any discovered security flaws will be temporarily giggled at, then promptly reported to its distributor. 

Very open to collaboration. Get a message to me via git and lets talk!

# Configuration
* nrf52850 w/ pca10056 processor
    * Note: Should port easily with other boards. See Zephyr supported boards.
* Zephyr RTOS
* Linux system
* Arm Embedded Toolchain
* Nordic JLink for flashing and cpu interface

# Building
* Assuming build on Linux type system. Can probably use cygwin on Windows, but not verified.
1. Setup Zephyr RTOS build environment. [See their documentation](https://docs.zephyrproject.org/latest/getting_started/getting_started.html)

2. Git clone blue-bandit
```
https://github.com/DC11011100/blue-bandit.git
```

3. Setup Zephyr with blue-bandit config:
```
cd blue-bandit
source scripts/zenv.sh <path-to-zephyr-base>
```
4. Create cmake generators. In this case: Makefiles:
  
 ```
 mkdir build && cd build
 cmake ..
 ```
 
 5. Build the hex and flash controller
 ```
 make && make flash 
 ```
 
 # Discovered Bluetooth Errata
 * BLE: CVE-2018-7252
   * Demonstrated by Damien Cauquil during Defcon 2018
   
 # Development Tips
 This is mostly for me >:D
 * [Zephyr + Bluez Configuration](https://docs.zephyrproject.org/1.13.0/subsystems/bluetooth/devel.html#bluetooth-bluez)
      * How configure laptop with BlueZ interface to Zephyr BLE
      * Enabling experimental functionality on bluez
      * Using btattach, btmon, etc, with Zephyr config
      
 * Bluetooth Stack Graphic
 
 ![Alt text](./nordic-bt-stack.svg)
      
 # Thread Breakdown
 #### USB UART (User)
   - [ ] Move to user thread
   - [ ] Import xDC RealTime terminal application thread. May need to refactor some freeRTOS API to Zephyr
   - [ ] Add terminal command discover bluetooth devices
 
 #### Bluetooth stack (kernel)
 
 #### Battery (Kernel)
   - [ ] Add kernel thread
   - [ ] Configure low power sleep mode
   - [ ] Add low-power preemptive shutdown protocols. Save/Close files, inform me that its running low (blink?)
     
 #### Terminal over Bluetooth (User)
   - [ ] Telecommands to manually control bluetooth profile
   - [ ] ? File transfer protocol to pull logged data
 
 
