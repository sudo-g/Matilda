#Matilda summary
Matilda is a component of Matador. It is the program that runs on the 
communication board of it. It receives incoming messages via bluetooth
and processes them. It delegates tasks to Shunt or Bash if necessary.

#Build configuration
This project is built using the CCS toolchain. CCS 6.x is recommended.

This project uses TI-RTOS 1.21 (do not use newer versions). The TI-RTOS 1.21
CCS add-on must be installed in order for this project to build. Download the
plugin from [here](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/mcusdk/index.html).

#Peripherals Exercised
##UART
Used to communicate with bluetooth module for both reception and transmission.
The index of the UART peripheral used communicate with the bluetooth module is
declared under `Board.h` as `Board_BT1`.

##I<sup>2</sup>C	
Used to communicate to Shunt and Bash. Matilda is the master of this bus.
The index of the I<sup>2</sup>C peripheral used for this inter-module bus is
declared under `Board.h` as `Board_INTER`.

##GPIO
The board integrated status LED is controlled by GPIO. It is declared under 
`Board.h` as `Board_STATUSLED`.

#Example Usage
The example only lights Board_LED0 as part of the initialization in main().

#Application Design Details
This examples is the same as the "Empty (Minimal)" example except many
developement and debug features are enabled. For example:
    - Logging is enabled
    - Assert checking is enabled
    - Kernel Idle task
    - Instrumented driver modules are used
    - Stack overflow checking
    - Default kernel heap is present

Please refer to the "Memory Footprint Reduction" section in the TI-RTOS User
Guide (spruhd4.pdf) for a complete and detailed list of the differences
between the empty minimal and empty projects.
