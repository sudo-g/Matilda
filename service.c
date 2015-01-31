/**
 * \file service.c
 * \brief Implements types and functions relevant to all services
 * \author George Xian
 * \version 0.1
 * \date 2014-11-30
 */

#include "service.h"

void eventHandlerInit(EventHandler* handler, void* instance, void (*callback)(UArg, UArg))
{
	handler->instance = instance;
	handler->callback = callback;
}

void eventListenerInit(EventListener* listener, EventHandler* handler)
{
	listener->handler = handler;
}
