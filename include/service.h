/**
 * \file service.h
 * \brief Declares functions and types relevant to all services
 * \author George Xian
 * \version 0.1
 * \date 2015-01-25
 */

#ifndef SERVICE
#define SERVICE

#include <xdc/std.h>
#include <ti/sysbios/knl/Queue.h>

#define SVC_NAME_LEN 8

/**
 * \struct EventHandler
 * \brief Provides context and routine to execute to handle event
 */
typedef struct
{
	void* instance;
	void (*callback)(UArg, UArg);
} EventHandler;

/**
 * \struct EventListener
 * \brief Binds callback functions to events
 */
typedef struct
{
	Queue_Elem _elem;
	EventHandler* handler;
} EventListener;

/**
 * \brief Populates required fields for CallbackContexts
 *
 * \param handler EventHandler instance to initialize
 * \param instance Pointer to application instance
 * \param callback Function to execute upon event
 */
void eventHandlerInit(EventHandler* handler, void* instance, void (*callback)(UArg, UArg));

/**
 * \brief Populates required fields for EventListeners
 *
 * \param listener EventListener instance to initialize
 * \param handler Pointer to EventHandler to associate with event
 */
void eventListenerInit(EventListener* listener, EventHandler* handler);

#endif
