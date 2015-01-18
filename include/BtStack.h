/**
 * \file BtStack.h
 * \brief Declares bluetooth stack service functions
 * \author George Xian
 * \version 0.1
 * \date 2014-11-30
 */

#ifndef BT_STACK
#define BT_STACK

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/UART.h>

#define KFP_FRAME_SIZE 14	//! No. of data bytes in Killalot frame protocol
#define KFP_WORST_SIZE 26	//! Maximum frame size if escape characters are used

#define SLIP_END 0xC0		//! SLIP END character
#define SLIP_ESC 0xDB		//! SLIP ESC character
#define SLIP_ESC_END 0xDC	//! Used to send 0xC0 when preceded by ESC character
#define SLIP_ESC_ESC 0xDD	//! Used to send 0xDB when preceded by ESC character

#define BTSTACK_SVCNAMELEN 8

typedef enum {KFPPRINTFORMAT_ASCII, KFPPRINTFORMAT_HEX} KfpPrintFormat;
typedef enum {BTBAUD_4800=4800, BTBAUD_9600=9600, BTBAUD_19200=19200, BTBAUD_38400=38400, BTBAUD_57600=57600, BTBAUD_115200=115200} BtBaud;

/**
 * \struct BtStack_Id
 * \brief Allows ID field of KFP to be accessed bytewise or wordwise of Thumb or ARM
 */
typedef union
{
	uint8_t b8[4];			//! bytewise
	uint16_t b16[2];		//! thumb wordwise
	uint32_t b32;			//! ARM wordwise
} BtStack_Id;

/**
 * \struct BtStack_Data
 * \brief Allows payload field of KFP to be accessed bytewise or wordwise of Thumb or ARM
 */
typedef union
{
	uint8_t b8[4];			//! bytewise
	uint16_t b16[2];		//! thumb wordwise
	uint32_t b32;			//! ARM wordwise
} BtStack_Data;

/**
 * \struct BtStack_Frame
 * \brief Represents KFP, allows access as structure or bytestream
 */
typedef union
{
	struct
	{
		BtStack_Id id;				//! 4 bytes ID
		BtStack_Data payload;		//! 8 bytes payload
	};
	uint8_t b8[KFP_FRAME_SIZE-2];	//! bytestream access
} BtStack_Frame;

/**
 * \typedef BtStack_callback
 * \brief Bluetooth stack service callback type
 */
typedef void (*BtStack_Callback)(const BtStack_Frame*);

/**
 * \struct BtStack_OnRecvListener
 * \brief Binder for application callbacks to receive events of this service
 */
typedef struct
{
	Queue_Elem _elem;
	BtStack_Callback callback;	//! Callback to execute on desired event
} BtStack_OnRecvListener;

/**
 * \struct BtStack_SvcHandle
 * \brief Handler to an instance of a BtStack service
 */
typedef struct
{
	char svcName[BTSTACK_SVCNAMELEN];	//! String identifier for debug and console

	UInt uartPeriphIndex;		//! UART peripheral connected to bluetooth module
	BtBaud baud;				//! Baud for UART peripheral

	Bool started;				//! Service status
	uint8_t rxSleep;			//! Number of ticks to sleep after complete frame

	UART_Handle btSocket;		//! Socket to UART driver
	Task_Handle rxTask;			//! Handle to the reception task
	int8_t rxPriority;			//! Reception task priority
	uint16_t rxStackSize;		//! Stack size allocation to reception task
	void (*rxFxn)(UArg, UArg);	//! Function reception task executes

	Queue_Handle recvEventQ;	//! Applications bound to this service
} BtStack_SvcHandle;

/**
 * \brief Initializes handle for default service configuration
 *
 * \param handle Pointer to handle
 * \param uartPeriphIndex UART peripheral connected to bluetooth to use
 * \param name String name for service instance (8 char limit)
 * \param baud Baud rate to use on UART peripheral connect to bluetooth
 */
void BtStack_handleInit(BtStack_SvcHandle* handle, char* name, UInt uartPeriphIndex, BtBaud baud);

/**
 * \brief Starts bluetooth stack service
 *
 * \param handle Pointer to the handle containing configuration for service
 * \return Returns 0 for success, -1 for task error
 */
int8_t BtStack_start(BtStack_SvcHandle* handle);

/**
 * \brief Stops bluetooth stack service
 *
 * \param handle Pointer to the handle of the service instance to be stopped
 * \return Returns 0 for success, -1 for failure
 */
int8_t BtStack_stop(BtStack_SvcHandle* handle);

/**
 * \brief Returns whether service has started
 *
 * \param handle Pointer to the handle of the service instance to be tested
 * \return Flag indicating whether service has started
 */
Bool BtStack_hasStarted(const BtStack_SvcHandle* handle);

/**
 * \brief Queues a frame send job
 *
 * \param handle Pointer to the handle of the service instance to send with
 * \param frame Frame to send
 * \returns Number of bytes written, -1 if failure
 */
int8_t BtStack_queue(const BtStack_SvcHandle* handle, const BtStack_Frame* frame);

/**
 * \brief Initializes bluetooth stack service listeners for applications
 *
 * \param appListener Pointer to the listener to initialize
 * \oaram callback Application callback to execute on event
 */
void BtStack_onRecvListenerInit(BtStack_OnRecvListener* appListener, BtStack_Callback callback);

/**
 * \brief Make application listener listen to on receive events of this service
 *
 * \param handle Pointer to the handle of the service instance to listen to
 * \param appListener Pointer to listener to register receive events
 */
void BtStack_registerOnRecv(const BtStack_SvcHandle* handle, BtStack_OnRecvListener* appListener);

/**
 * \brief Stop application listener from listening to on receive events of this service
 *
 * \param handle Pointer to the handle of the service instance to stop listening
 * \param appListener Pointer to listener to remove receive events
 */
void BtStack_removeOnRecv(const BtStack_SvcHandle* handle, BtStack_OnRecvListener* appListener);

/**
 * \brief Prints KFP frames to the console
 *
 * \param frame Frame to print
 * \param format Print frame as a series of ASCII characters or hexadecimal numbers
 */
void BtStack_framePrint(const BtStack_Frame* frame, KfpPrintFormat format);


#endif
