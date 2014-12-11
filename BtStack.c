/**
 * \file BtStack.c
 * \brief Implements btStack service
 * \author George Xian
 * \version 0.1
 * \date 2014-11-30
 */

#include "BtStack.h"

#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/System.h>
#include <string.h>
#include "Board.h"

#define DEFAULT_RX_PRIORITY 10			//! Default priority of reception task
#define DEFAULT_RX_STACK 2048			//! Default stack size of reception task
#define DEFAULT_UART_BAUD 115200		//! Default baud rate for UART

static Bool hasStart = FALSE;					//! Task started status
static Task_Handle rxTask = NULL;				//! Handle to the reception task
static int8_t rxPriority = DEFAULT_RX_PRIORITY;	//! Priority of reception task
static uint16_t rxStackSize = DEFAULT_RX_STACK;	//! Stack size of reception task
static BtStack_Callback rxCallback = NULL;		//! Function to call on receive event
static Semaphore_Handle rxSem = NULL;			//! Return CPU time from reception task
static char tempRx[1];							//! Reception buffer

static uint32_t uartBaud = DEFAULT_UART_BAUD;	//! Baud rate to initiate UART peripheral to

/**
 * \brief Function executed by the reception task
 */
void rxFxn(UArg unused0, UArg unused1);

/**
 * \brief Callback on an on receive event
 *
 * \param handle UartHandle concerned with the reception
 * \param received Character received
 */
void onReceive(UART_Handle handle, char* received);

int8_t BtStack_start(void)
{
	if (rxTask != NULL)
	{
		// service already started
		return -1;
	}

	// generic error block
	Error_Block eb;
	Error_init(&eb);

	// create receive semaphore
	rxSem = Semaphore_create(1, NULL, &eb);

	// creating the task
	Task_Params params;
	Task_Params_init(&params);
	params.instance->name = "btStack::rx";
	params.priority = rxPriority;
	params.stackSize = rxStackSize;

	rxTask = Task_create((Task_FuncPtr) rxFxn, &params, &eb);
	if (rxTask == NULL)
	{
		return -2;
	}
	else
	{
		hasStart = TRUE;
		return 0;
	}
}

int8_t BtStack_stop(void)
{
	Task_delete(&rxTask);
	rxSem = NULL;
	hasStart = FALSE;

	return 0;
}

Bool BtStack_hasStarted(void)
{
	return hasStart;
}

Bool BtStack_hasCallback(void)
{
	if (rxCallback == NULL)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int8_t BtStack_attachCallback(BtStack_Callback callback)
{
	if (BtStack_hasCallback())
	{
		return -1;
	}
	else
	{
		rxCallback = callback;
		return 0;
	}
}

int8_t BtStack_removeCallback(void)
{
	if (BtStack_hasCallback())
	{
		rxCallback = NULL;
		return 0;
	}
	else
	{
		return -1;
	}
}

int8_t BtStack_push(const BtStack_Frame* frame)
{
	// special character declarations
	const char escapedEnd[] = {SLIP_ESC, SLIP_ESC_END};	// escaped 0xC0
	const char escapedEsc[] = {SLIP_ESC, SLIP_ESC_ESC};	// escaped 0xDB

	char sendStream[KFP_WORST_SIZE];
	uint8_t sentChar = 0;

	// append start character
	sendStream[0] = SLIP_END;
	sentChar++;

	// iterate through frame adding ESC characters where necessary
	uint8_t i;
	for (i=0; i<KFP_FRAME_SIZE-2; i++)
	{
		switch(frame->b8[i])
		{
		case(SLIP_END):
				// escape END character
				strncat(sendStream, escapedEnd, 2);
				sentChar+=2;
				break;
		case(SLIP_ESC):
				// escape ESC character
				strncat(sendStream, escapedEsc, 2);
				sentChar+=2;
				break;
		default:
				sendStream[sentChar] = frame->b8[i];
				sentChar++;
		}
	}

	// append end character
	sendStream[sentChar] = SLIP_END;
	sentChar++;

	// prepare UART socket
	UART_Handle s;
	UART_Params params;
	UART_Params_init(&params);
	params.baudRate = uartBaud;
	params.writeMode = UART_MODE_BLOCKING;
	params.writeDataMode = UART_DATA_BINARY;
	params.readDataMode = UART_DATA_BINARY;
	params.readReturnMode = UART_RETURN_FULL;
	params.readEcho = UART_ECHO_OFF;
	s = UART_open(Board_BT1, &params);

	// write to socket and close once complete
	int8_t ret = UART_write(s, sendStream, sentChar);
	UART_close(s);

	return ret;
}

void BtStack_framePrint(const BtStack_Frame* frame, KfpPrintFormat format)
{
	System_printf("ID =");
	uint8_t i;
	for (i=0; i<4; i++)
	{
		System_printf(" %u", frame->id.b8[i]);
	}

	System_printf(" Data =");
	for (i=0; i<8; i++)
	{
		if (format == KFPPRINTFORMAT_ASCII)
		{
			System_printf("%c", frame->payload.b8[i]);
		}
		else if (format == KFPPRINTFORMAT_HEX)
		{
			System_printf("%x", frame->payload.b8[i]);
		}
	}
	System_flush();
}

void rxFxn(UArg param0, UArg param1)
{
	// Declare state variables
	Bool inFrame = FALSE;
	uint8_t frIndex = 0;

	// Buffers
	BtStack_Frame tempFr;

	while(TRUE)
	{
		// semaphore must always be 0 on read so that it will wait until read is finished
		Semaphore_pend(rxSem, BIOS_WAIT_FOREVER);

		// open socket
		UART_Handle s;
		UART_Params params;
		UART_Params_init(&params);
		params.baudRate = uartBaud;
		params.writeDataMode = UART_DATA_BINARY;
		params.readMode = UART_MODE_CALLBACK;
		params.readCallback = (UART_Callback) onReceive;
		params.readDataMode = UART_DATA_BINARY;
		params.readReturnMode = UART_RETURN_FULL;
		params.readEcho = UART_ECHO_OFF;
		s = UART_open(Board_BT1, &params);

		// read UART buffer and decode
		UART_read(s, NULL, 1);
		Semaphore_pend(rxSem, BIOS_WAIT_FOREVER);

		switch(tempRx[0])
		{
		case(SLIP_END):
				if (inFrame)
				{
					if (frIndex == (KFP_FRAME_SIZE-2))
					{
						// end of frame, call callback to interpret it
						if (rxCallback != NULL)
						{
							rxCallback(&tempFr);
						}
					}

					// ignore corrupt frames
					frIndex = 0;
					inFrame = FALSE;

					break;
				}
				else
				{
					inFrame = TRUE;		// start new frame
					frIndex = 0;
					break;
				}
		case(SLIP_ESC):
				if (inFrame)
				{
					UART_read(s, tempRx, 1);
					switch(tempRx[0])
					{
					case(SLIP_ESC_END):
							tempFr.b8[frIndex] = SLIP_END;
							frIndex++;
							break;
					case(SLIP_ESC_ESC):
							tempFr.b8[frIndex] = SLIP_ESC;
							frIndex++;
							break;
					default:
						break;	// invalid post ESC character
					}
					break;
				}
				else
				{
					break;		// ignore corrupt frames
				}
		default:
			if (inFrame)
			{
				// standard character store
				tempFr.b8[frIndex] = tempRx[0];
				frIndex++;
				break;
			}
		}
	}
}

void onReceive(UART_Handle handle, char* received)
{
	strncpy(tempRx, received, 1);
	Semaphore_post(rxSem);
}
