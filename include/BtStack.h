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

#define KFP_FRAME_SIZE 14	//! No. of data bytes in Killalot frame protocol
#define KFP_WORST_SIZE 26	//! Maximum frame size if escape characters are used

#define SLIP_END 0xC0		//! SLIP END character
#define SLIP_ESC 0xDB		//! SLIP ESC character
#define SLIP_ESC_END 0xDC	//! Used to send 0xC0 when preceded by ESC character
#define SLIP_ESC_ESC 0xDD	//! Used to send 0xDB when preceded by ESC character

typedef enum {KFPPRINTFORMAT_ASCII, KFPPRINTFORMAT_HEX} KfpPrintFormat;

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
	uint8_t b8[KFP_FRAME_SIZE-2];		//! bytestream access
} BtStack_Frame;

/**
 * \typedef BtStack_callback
 * \brief Bluetooth stack service callback type
 */
typedef void (*BtStack_Callback)(const BtStack_Frame*);


/**
 * \brief Starts bluetooth stack service
 *
 * \return Returns 0 for success, -1 if service already started and -2 for reception thread start failure
 */
int8_t BtStack_start(void);

/**
 * \brief Stops bluetooth stack service
 *
 * \return Returns 0 for success, -1 for failure
 */
int8_t BtStack_stop(void);

/**
 * \brief Returns whether service has started
 *
 * \return Flag indicating whether service has started
 */
Bool BtStack_hasStarted(void);

/**
 * \brief Checks whether this service has callback attached
 *
 * \return Flag indicating whether callback is attached to this service
 */
Bool BtStack_hasCallback(void);

/**
 * \brief Attach callback for reception event
 *
 * \param callback Callback to execute on reception event
 * \return Returns 0 if callback was NULL, -1 if a callback was already attached
 */
int8_t BtStack_attachCallback(BtStack_Callback callback);

/**
 * \brief Removes reception event callback
 *
 * \return Returns 0 for success, -1 if existing callback was NULL
 */
int8_t BtStack_removeCallback(void);

/**
 * \brief Pushes a frame to the back of the send queue
 *
 * \param frame Frame to send
 * \returns Number of bytes written
 */
int8_t BtStack_push(const BtStack_Frame* frame);

/**
 * \brief Prints KFP frames to the console
 *
 * \param frame Frame to print
 * \param format Print frame as a series of ascii characters or hexadecimal numbers
 */
void BtStack_framePrint(const BtStack_Frame* frame, KfpPrintFormat format);


#endif
