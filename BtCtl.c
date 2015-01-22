/**
 * \file BtCtl.c
 * \brief Implements bluetooth vehicle control application function
 * \author George Xian
 * \version 0.1
 * \date 2014-12-10
 */

#include "BtCtl.h"

#include <string.h>

void BtCtl_handleInit(BtCtl_AppHandle* ctlHandle, char* name, BtStack_SvcHandle* btInstance, PwrMgmt_SvcHandle* pwrInstance)
{
	ctlHandle->btStackInstance = btInstance;
	ctlHandle->pwrMgmtInstance = pwrInstance;

	strncpy(ctlHandle->appName, name, BTCTL_APPNAMELEN);
}

int BtCtl_start(BtCtl_AppHandle* ctlInstance, const BtStack_SvcHandle* btInstance)
{
	BtStack_onRecvListenerInit(&ctlInstance->rxListener, (BtStack_Callback) BtCtl_rxCallback);
	BtStack_registerOnRecv(btInstance, &(ctlInstance->rxListener));

	ctlInstance->started = TRUE;

	return 0;
}

int BtCtl_stop(BtCtl_AppHandle* ctlInstance)
{
	BtStack_removeOnRecv(ctlInstance->btStackInstance, &(ctlInstance->rxListener));

	ctlInstance->started = FALSE;
	return 0;
}

void BtCtl_rxCallback(void* instance, const BtStack_Frame* frame)
{
	BtCtl_AppHandle* appInstance = (BtCtl_AppHandle*) instance;

	if (frame->id.b8[0] == 1)
	{
		// frame identified as first 8 channels, which are analog
		if (frame->id.b8[3] == 0)
		{
			// channel 2 and 3 of Phantom is yaw and power respectively
			PwrMgmt_drive(appInstance->pwrMgmtInstance, frame->payload.b8[3], frame->payload.b8[2]);
		}

		// frame identiified as last 8 channels, which are digital
		// for digital channels, 127 is on and 0 is off
		if (frame->id.b8[3] == 8)
		{
			// channel 10 of Phantom is toggle switches
			if (frame->payload.b8[2] == 127)
			{
				PwrMgmt_weapon(appInstance->pwrMgmtInstance, WEAPON_1, 1);
			}
			else if (frame->payload.b8[2] == 0)
			{
				PwrMgmt_weapon(appInstance->pwrMgmtInstance, WEAPON_1, 0);
			}
		}
	}
}
