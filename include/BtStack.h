/**
 * \file BtStack.h
 * \brief Declares btStack service functions
 * \author George Xian
 * \version 0.1
 * \date 2014-11-30
 */

#ifndef BT_STACK
#define BT_STACK

#include <ti/drivers/UART.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#define KFP_FRAME_SIZE 14	//! No. of data bytes in Killalot frame protocol
#define KFP_WORST_SIZE 26	//! Maximum frame size if escape characters are used

#define MAX_RX_CALLBACKS 4	//! Maximum number of callbacks attachable to receive event

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
 * \struct BtStack_SvcHandle
 * \brief Handler to an instance of a BtStack service
 */
typedef struct
{
	UInt uartPeriphHandle;		//! UART peripheral connected to bluetooth module
	BtBaud baud;				//! Baud rate of UART peripheral to use

	Bool started;				//! Service status
	Task_Handle rxTask;			//! Handle to the reception task
	int8_t rxPriority;			//! Reception task priority
	uint16_t rxStackSize;		//! Stack size allocation to reception task
	void (*rxFxn)(UArg, UArg);	//! Function reception task executes

	BtStack_Callback rxCallbacks[MAX_RX_CALLBACKS];	//! Callbacks on receive event
	uint8_t numCallbacks;
} BtStack_SvcHandle;

/**
 * \brief Initializes handle for default service configuration
 *
 * \param handle Pointer to handle
 * \param uartPeriphHandle UART peripheral connected to bluetooth to use
 */
void BtStack_handleInit(BtStack_SvcHandle* handle, UInt uartPeriphHandle);

/**
 * \brief Starts bluetooth stack service
 *
 * \param handle Pointer to the handle containing configuration for service
 */
void BtStack_start(BtStack_SvcHandle* handle);

/**
 * \brief Stops bluetooth stack service
 *
 * \param handle Pointer to the handle of the service instance to be stopped
 * \return Returns 0 for success, -1 for failure
 */
void BtStack_stop(BtStack_SvcHandle* handle);

/**
 * \brief Returns whether service has started
 *
 * \param handle Pointer to the handle of the service instance to be tested
 * \return Flag indicating whether service has started
 */
Bool BtStack_hasStarted(const BtStack_SvcHandle* handle);

/**
 * \brief Attach callback for reception event
 *
 * \param handle Pointer to the handle of the service instance to modify
 * \param callback Callback to execute on reception event
 * \return Returns 0 for success, -1 if callback queue is full, -2 if service running
 */
int8_t BtStack_attachCallback(BtStack_SvcHandle* handle, BtStack_Callback callback);

/**
 * \brief Removes all reception event callbacks
 *
 * \param handle Pointer to the handle of the service instance to modify
 * \return Returns 0 for success, -1 if service running
 */
int8_t BtStack_removeCallbacks(BtStack_SvcHandle* handle);

/**
 * \brief Pushes a frame to the back of the send queue
 *
 * \param handle Pointer to the handle of the service instance to send with
 * \param frame Frame to send
 * \returns Number of bytes written, -1 if failure
 */
int8_t BtStack_push(const BtStack_SvcHandle* handle, const BtStack_Frame* frame);

/**
 * \brief Prints KFP frames to the console
 *
 * \param frame Frame to print
 * \param format Print frame as a series of ASCII characters or hexadecimal numbers
 */
void BtStack_framePrint(const BtStack_Frame* frame, KfpPrintFormat format);


#endif
