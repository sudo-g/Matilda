/**
 * \file BtCtl.c
 * \brief Implements bluetooth vehicle control application function
 * \author George Xian
 * \version 0.1
 * \date 2014-12-10
 */

#include "BtCtl.h"

#include "PwrMgmt.h"

void BtCtl_rxCallback(const BtStack_Frame* frame)
{
	// frame identified as first 8 channels, which are analog
	if (frame->id.b8[0] == 1)
	{
		// channel 2 and 3 of Phantom is yaw and power respectively
		PwrMgmt_drive(frame->payload.b8[3], frame->payload.b8[2]);
	}

	// frame identiified as last 8 channels, which are digital
	// for digital channels, 127 is on and 0 is off
	if (frame->id.b8[0] == 8)
	{
		// channel 10 of Phantom is toggle switches
		if (frame->payload.b8[2] == 127)
		{
			PwrMgmt_weapon(WEAPON_1, 1);
		}
		else
		{
			PwrMgmt_weapon(WEAPON_1, 0);
		}
	}
}
