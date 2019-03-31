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
2. Setup Zephyr with blue-bandit config:
```
source <blue-bandit root>/zenv
```
3. Create cmake generators. In this case: Makefiles:
  
 ```
 mkdir build && cd build
 cmake <blue-bandit root>
 ```
 
 4. Flash the controller
 ```
 <blud-bandit root>/scripts/flash.sh 
 ```
 
 # Errate to be abused
 * BLE: CVE-2018-7252
   * Announced during Defcon 2018.
 
