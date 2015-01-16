/**
 * \file BtStack.c
 * \brief Implements btStack service
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

/**
 * \brief Function reception task executes
 */
void rxFxn(UArg param0, UArg param1);

void BtStack_handleInit(BtStack_SvcHandle* handle, UInt uartPeriphHandle)
{
	// must be user set
	handle->uartPeriphHandle = uartPeriphHandle;

	// optionally set
	handle->baud = DEFAULT_BT_BAUD;
	handle->rxPriority = DEFAULT_RX_PRIORITY;
	handle->rxStackSize = DEFAULT_RX_STACK;

	// read only
	handle->numCallbacks = 0;
}

void BtStack_start(BtStack_SvcHandle* handle)
{
	Task_Params taskParams;
	Task_Params_init(&taskParams);
	taskParams.stackSize = handle->rxStackSize;
	taskParams.priority = handle->rxPriority;
	handle->rxTask = Task_create((Task_FuncPtr) rxFxn, &taskParams, NULL);
	if (handle->rxTask != NULL)
	{
		handle->started = TRUE;
	}
}

void BtStack_stop(BtStack_SvcHandle* handle)
{
	Task_delete(&handle->rxTask);
	handle->started = FALSE;
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
	while(TRUE)
	{
		//TODO:
	}
}

void onReceive(UART_Handle handle, char* received)
{
	strncpy(tempRx, received, 1);
	Semaphore_post(rxSem);
}
