/**
 * \file BtCtl.h
 * \brief Declares bluetooth vehicle control application functions
 * \author George Xian
 * \version 0.1
 * \date 2014-12-10
 */

#ifndef BT_CTL
#define BT_CTL

#include "BtStack.h"
#include "PwrMgmt.h"

/**
 * \struct BtCtl_AppHandle
 * \brief Handler to an instance of a BtCtl app
 */
typedef struct BtCtl_AppHandle BtCtl_AppHandle;

/**
 * \brief Initializes handle for default application configuration
 *
 * \param ctlHandle Handle to the bluetooth control app instance
 * \param btInstance Handle to the bluetooth stack service instance
 * \param pwrInstance Handle to the power management service instance
 */
void BtCtl_handleInit(BtCtl_AppHandle* ctlHandle, BtStack_SvcHandle* btInstance, PwrMgmt_SvcHandle* pwrInstance);

/**
 * \brief Starts bluetooth control application
 *
 * \param ctlInstance Pointer to the handle containing configuration to this app
 * \return Returns 0 for success, -1 for failure
 */
int BtCtl_start(BtCtl_AppHandle* ctlInstance);

/**
 * \brief Stops bluetooth control application
 *
 * \param ctlInstance Pointer to the handle of the application instance to be stopped
 * \return Returns 0 for success, -1 for failure
 */
int BtCtl_stop(BtCtl_AppHandle* ctlInstance);

/**
 * \brief Sets drive speed according to received frame
 *
 * \param instance BtCtl instance the service is signalling
 * \param frame Bluetooth frame received
 */
void BtCtl_rxCallback(void* instance, const BtStack_Frame* frame);

#endif
