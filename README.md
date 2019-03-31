# Configuration
* nrf52850 w/ pca10056 processor
    * Note: Should port easily with other boards. See Zephyr supported boards.
* Zephyr RTOS
* Linux system

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
  
# blue-bandit
Bluetooth 4 &amp; 5 packet sniffer and pen-tester
