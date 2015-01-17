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

typedef enum {KFPPRINTFORMAT_ASCII, KFPPRINTFORMAT_HEX} KfpPrintFormat;
typedef enum {BTBAUD_9600=9600, BTBAUD_19200=19200, BTBAUD_38400=38400, BTBAUD_57600=57600, BTBAUD_115200=115200} BtBaud;

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
 * \struct BtStack_Listener
 * \brief Wraps application callbacks for use with queue
 */
typedef struct
{
	Queue_Elem _elem;
	BtStack_Callback callback;
} BtStack_Listener;

/**
 * \struct BtStack_SvcHandle
 * \brief Handler to an instance of a BtStack service
 */
typedef struct
{
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
 */
void BtStack_handleInit(BtStack_SvcHandle* handle, UInt uartPeriphIndex);

/**
 * \brief Starts bluetooth stack service
 *
 * \param handle Pointer to the handle containing configuration for service
 * \return Returns 0 for success, -1 if UART socket failed to open, -2 if reception task failed to start
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
 * \brief Adds an application to listen to receive events of this service
 *
 * \param handle Pointer to the handle of the service instance to add listener for
 * \param appListener Pointer to listener wrapping application callback add
 */
void BtStack_addListener(const BtStack_SvcHandle* handle, BtStack_Listener* appListener);

/**
 * \brief Removes an application from listening to receive events of this service
 *
 * \param handle Pointer to the handle of the service instance to remove listener from
 * \param appListener Pointer to listener wrapping application callback to remove
 */
void BtStack_removeListener(const BtStack_SvcHandle* handle, BtStack_Listener* appListener);

/**
 * \brief Prints KFP frames to the console
 *
 * \param frame Frame to print
 * \param format Print frame as a series of ASCII characters or hexadecimal numbers
 */
void BtStack_framePrint(const BtStack_Frame* frame, KfpPrintFormat format);


#endif
