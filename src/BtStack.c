/**
 * \file btStack.c
 * \brief Implements btStack service
 * \author George Xian
 * \version 0.1
 * \date 2014-11-30
 */

#include "BtStack.h"

#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>

#define DEFAULT_RX_PRIORITY = 10		//! Default priority of reception task
#define DEFAULT_RX_STACK = 2048			//! Default stack size of reception task
#define DEFAULT_UART_BAUD = 115200		//! Default baud rate for UART

static Bool hasStart = FALSE;					//! Task started status
static Task_Handle rxTask = NULL;				//! Handle to the reception task
static int8_t rxPriority = DEFAULT_RX_PRIORITY;	//! Priority of reception task
static uint16_t rxStackSize = DEFAULT_RX_STACK;	//! Stack size of reception task
static BtStack_Callback rxCallback = NULL;		//! Function to call on receive event

static uint32_t uartBaud = DEFAULT_UART_BAUD;	//! Baud rate to initiate UART peripheral to

/**
 * \brief Function executed by the reception task
 */
void rxFxn(UArg unused0, UArg unused1);

int8_t BtStack_start(void)
{
	if (rxTask != NULL)
	{
		// service already started
		return -1;
	}

	// creating the task
	Task_Params params;
	Task_Params_init(&params);
	params.instance->name = "btStack::rx";
	params.priority = rxPriority;
	params.stackSize = rxStackSize;

	Error_Block eb;
	Error_init(&eb);

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
	hasStart = FALSE;

	return 0;
}

Bool BtStack_hasStarted(void)
{
	return hasStarted;
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

int8_t BtStack_attachCallback(BtStack_callback callback)
{
	if (btStack_hasCallback())
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
	if (btStack_hasCallback())
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
	for (uint8_t i=0; i<KFP_FRAME_SIZE-2; i++)
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

void BtStack_framePrint(const BtStackFrame* frame, KfpPrintFormat format)
{
	System_printf("ID =");
	for (uint8_t i=0; i<4; i++)
	{
		System_printf(" %u", frame->id.b8[i]);
	}

	System_printf(" Data =");
	for (uint8_t i=0; i<8; i++)
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
