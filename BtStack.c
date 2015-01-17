/**
 * \file BtStack.c
 * \brief Implements bluetooth stack service
 * \author George Xian
 * \version 0.1
 * \date 2014-11-30
 */

#include "BtStack.h"

#include <xdc/runtime/Error.h>

#include <xdc/runtime/System.h>
#include <string.h>
#include "Board.h"

#define DEFAULT_RX_PRIORITY 10			//! Default priority of reception task
#define DEFAULT_RX_STACK 2048			//! Default stack size of reception task
#define DEFAULT_BT_BAUD BTBAUD_115200	//! Default baud rate for UART peripheral connected to bluetooth module
#define DEFAULT_RX_SLEEP 50				//! Number of ticks between frame complete ticks

/**
 * \brief Function reception task executes
 *
 * \param handle Service instance handle hosting the task
 */
void rxFxn(UArg handle, UArg param1);

void BtStack_handleInit(BtStack_SvcHandle* handle, UInt uartPeriphIndex)
{
	// must be user set
	handle->uartPeriphIndex = uartPeriphIndex;

	// optionally set
	handle->baud = DEFAULT_BT_BAUD;
	handle->rxPriority = DEFAULT_RX_PRIORITY;
	handle->rxStackSize = DEFAULT_RX_STACK;
	handle->rxSleep = DEFAULT_RX_SLEEP;
}

int8_t BtStack_start(BtStack_SvcHandle* handle)
{
	// prepare UART driver socket
	UART_Params params;
	UART_Params_init(&params);
	params.baudRate = handle->baud;
	params.writeDataMode = UART_DATA_BINARY;
	params.readDataMode = UART_DATA_BINARY;
	params.readReturnMode = UART_RETURN_FULL;
	params.readEcho = UART_ECHO_OFF;
	handle->btSocket = UART_open(handle->uartPeriphIndex, &params);
	if (handle->btSocket == NULL)
	{
		return -1;
	}

	// setup listener queue
	handle->recvEventQ = Queue_create(NULL, NULL);

	// setup reception task
	Task_Params taskParams;
	Task_Params_init(&taskParams);
	taskParams.stackSize = handle->rxStackSize;
	taskParams.priority = handle->rxPriority;
	taskParams.arg0 = (UArg) &handle;
	handle->rxTask = Task_create((Task_FuncPtr) rxFxn, &taskParams, NULL);
	if (handle->rxTask != NULL)
	{
		handle->started = TRUE;
		return 0;
	}
	else
	{
		return -2;
	}
}

int8_t BtStack_stop(BtStack_SvcHandle* handle)
{
	Task_delete(&handle->rxTask);
	Queue_delete(&handle->recvEventQ);
	UART_close(handle->btSocket);
	handle->started = FALSE;

	return 0;
}

Bool BtStack_hasStarted(const BtStack_SvcHandle* handle)
{
	return handle->started;
}

int8_t BtStack_queue(const BtStack_SvcHandle* handle, const BtStack_Frame* frame)
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

	// write to socket and close once complete
	int8_t ret = UART_write(handle->btSocket, sendStream, sentChar);

	return ret;
}

void BtStack_addListener(const BtStack_SvcHandle* handle, BtStack_Listener* appListener)
{
	Queue_put(handle->recvEventQ, &appListener->_elem);
}

void BtStack_removeListener(const BtStack_SvcHandle* handle, BtStack_Listener* appListener)
{
	Queue_remove(&appListener->_elem);
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

void rxFxn(UArg handle, UArg param1)
{
	BtStack_SvcHandle* btStackHandle = (BtStack_SvcHandle*) handle;

	bool inFrame = FALSE;
	uint8_t frIndex = 0;
	BtStack_Frame recvFrame;

	while(TRUE)
	{
		char rxBuffer[1];
		char escRxBuffer[1];
		UART_read(btStackHandle->btSocket, rxBuffer, 1);
		switch(rxBuffer[0])
		{
		case(SLIP_END):
				if (inFrame)
				{
					if (frIndex == (KFP_FRAME_SIZE-2))
					{
						// end of frame, signal applications
						while(!Queue_empty(btStackHandle->recvEventQ))
						{
							BtStack_Listener* appListener = Queue_dequeue(btStackHandle->recvEventQ);
							appListener->callback(&recvFrame);
						}
					}

					// reset states, ignore corrupt frames
					frIndex = 0;
					inFrame = FALSE;
					Task_sleep(btStackHandle->rxSleep);
					break;
				}
				else
				{
					// start a new frame
					inFrame = TRUE;
					frIndex = 0;
					break;
				}
		case(SLIP_ESC):
				if (inFrame)
				{
					UART_read(btStackHandle->btSocket, escRxBuffer, 1);
					switch(escRxBuffer[0])
					{
					case(SLIP_ESC_END):
							recvFrame.b8[frIndex] = SLIP_END;
							frIndex++;
							break;
					case(SLIP_ESC_ESC):
							recvFrame.b8[frIndex] = SLIP_ESC;
							frIndex++;
							break;
					default:
							break;	// invalid escape character
					}
				}
				else
				{
					break;	// ignore stray characters
				}
		default:
				if (inFrame)
				{
					// standard character receive
					recvFrame.b8[frIndex] = rxBuffer[0];
					frIndex++;
				}
				else
				{
					break;	// ignore stray characters
				}
		}
	}
}
