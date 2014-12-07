#Matilda summary
Matilda is a component of Matador. It is the program that runs on the 
communication board of it. It receives incoming messages via bluetooth
and processes them. It delegates tasks to Shunt or Bash if necessary.

#Build configuration
This project is built using the CCS toolchain. CCS 6.x is recommended.

This project uses TI-RTOS 1.21 (do not use newer versions). The TI-RTOS 1.21
CCS add-on must be installed in order for this project to build. Download the
plugin from [here](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/mcusdk/index.html).
Once TI-RTOS is configured, this project should build without error.

#Note to Contributors
This project uses gitflow. Use of SourceTree is recommended to keep branch
usage consistent. To add features to this project, create an issue on
this repository's space on GitHub. The personnel adding this feature should
create a new branch on develop (SourceTree manages to automatically) and name
the branch with this syntax: "<issue no.>_<issue name>" (although spaces need
to be substituted with "_" as git doesn't allow spaces in branch names). Every 
commit to that branch should then have the following syntax: "#<issue no.> <issue_name>: <message>".

#Peripherals Exercised
##UART
Used to communicate with the bluetooth module for both reception and transmission.
The index of the UART peripheral used for communicate with the bluetooth module is
declared under `Board.h` as `Board_BT1`.

##I<sup>2</sup>C	
Used to communicate to Shunt and Bash. Matilda is the master of this bus.
The index of the I<sup>2</sup>C peripheral used for this inter-module bus is
declared under `Board.h` as `Board_INTER`.

##GPIO
The board integrated status LED is controlled by GPIO. It is declared under 
`Board.h` as `Board_STATUSLED`.

#Architecture
Matilda uses a multi-layered service orientated architecture. With exception,
it encompasses three layers:
*Hardware layer
*Service layer
*Application layer

This design principle encourages modules from any layer to only import modules
from layers below it.

##Hardware layer
TI-RTOS drivers the hardware layer. Contributers cannot add modules for this layer
unless they are creating subsititute modules for TI-RTOS drivers in the case that
they are faulty. 

This layer includes drivers for UART, I<sup>2</sup>C and GPIO. 

##Service layer
The layer above the hardware layer is the service layer. Purposes of this layer
include:
*Process unstructured data incoming from the hardware layer into abstract structured data
*Convert structured data from application into bytestreams for use with the hardware layer
*Forward to the appropriate application module based on data received
 
This layer includes bluetooth message handling and interface API to Shunt and Bash. 

Services are allowed to have multiple indefinitely runnings threads.
All header files to services must be placed in the
'include' folder. All source files to services must be placed in root.

##Application layer
The layer above the service layer is the application layer. The purpose of this
layer is to perform an operation based on the data received (such as calling 
other services). Such as the drive application which interprets throttle levels 
from the bluetooth service and delegates the power management service to control
the wheels appropriately.

This layer includes bluetooth drive control, camera image transmission and console.

Applications are encouraged to use threads on service callbacks.
All header files to applications must be placed in the 'include' folder. All
source files to application must be placed in root.



