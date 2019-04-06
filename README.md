# Description
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
git clone https://github.com/DC11011100/blue-bandit.git
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
 
 5. Build the hex then flash controller
 ```
 make && make flash 
 ```
 
# Modules
#### USB UART Terminal (User)
###### Entering shell
'-con' turns on terminal coloring. Note you may need to point to a different /dev/tty device
```
minicom -D /dev/ttyACM0 -b 115200 -con
```
###### TODO
   - [X] Implement uart shell
   - [ ] Move to user thread
   - [ ] Add terminal command to discover bluetooth devices
   - [ ] Add terminal command for spawning kernel space BT profile thread
   - [ ] Decouple inputs to terminal so IO data streams (xin xout) can come from various HW such as BT, USB, etc.


#### BLE UART Terminal (User)
Secure connection enforced via predetermined key. You will need some special software for this...which I may end up
creating if necessary. TBD
 
###### TODO
   - [ ] Enable secure connection peripheral ble service
   - [ ] Reuse shell from USB UART Shell
        - [ ] Intercept trasport layer and replace USB UART --> BLE UART
   - [ ] Nice 2 Have: Setup Object Transfer Service for gatt. Meant to offload big data that may otherwise inhibit the
         bluetooth shell
        - [ ] Setup parallel bluetooth service thread with object pointer

#### Bluetooth Sniffer (Kernel)
Daemon initiated via system call from user space terminal command.
###### TODO
   - [ ] Terminal command
   - [ ] Bluetooth sniffing capability
   - [ ] Bluetooth profile imposter
   - [ ] Log generator
   - [ ] Man in the middle ?
        - [ ] See Damien Cauquil presentation in Defcon 2018
   
#### Battery (Kernel)
Kernel daemon which broadcasts low-battery/shutoff events and performs preemptive safety functions upon these events.


###### TODO
   - [ ] Add kernel thread to monitor battery level
   - [ ] Configure low power sleep mode
   - [ ] Add low-power preemptive shutdown protocols. Save/Close files, inform me that its running low (blink?)
 
 
 
# Misc 
##### Discovered Bluetooth Errata
 * BLE: CVE-2018-7252
   * Demonstrated by Damien Cauquil during Defcon 2018
   
##### Development Tips
 * [Zephyr + Bluez Configuration](https://docs.zephyrproject.org/1.13.0/subsystems/bluetooth/devel.html#bluetooth-bluez)
      * How configure laptop with BlueZ interface to Zephyr BLE
      * Enabling experimental functionality on bluez
      * Using btattach, btmon, etc, with Zephyr config
      
 * Bluetooth Stack Graphic
 
<p align="center">
  <img width="460" height="300" src="nordic-bt-stack.svg">
</p>

 
 * [Bluetooth Gatt Services](https://www.bluetooth.com/specifications/gatt/services)
