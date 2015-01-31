/**
 * \file PwrMgmt.c
 * \brief Implements power management service
 * \author George Xian
 * \version 0.1
 * \date 2014-12-06
 */

#include "PwrMgmt.h"

#include <ti/drivers/I2C.h>
#include <string.h>
#include <xdc/runtime/System.h>

#define BATTERY_REQUEST_CODE 131
typedef enum {DRV_PWR = 101, DRV_YAW = 102} DrvComponent;

typedef union
{
	struct
	{
		int8_t component;
		int8_t magnitude;
	};
	UChar b8[2];
} PwrMgmt_Message;

void PwrMgmt_handleInit(PwrMgmt_SvcHandle* pwrHandle, char* name, unsigned int i2cPeriphIndex, unsigned int pwrBoardAddr)
{
	pwrHandle->i2cPeriphIndex = i2cPeriphIndex;
	pwrHandle->pwrBoardAddr = pwrBoardAddr;
	strncpy(pwrHandle->svcName, name, SVC_NAME_LEN);

	pwrHandle->started = FALSE;
}

int PwrMgmt_start(PwrMgmt_SvcHandle* pwrHandle)
{
	pwrHandle->started = TRUE;

	return 0;
}

int PwrMgmt_stop(PwrMgmt_SvcHandle* pwrInstance)
{
	pwrInstance->started = FALSE;

	return 0;
}

int PwrMgmt_drive(PwrMgmt_SvcHandle* pwrInstance, int power, int yaw)
{
	// Generate transaction messages
	PwrMgmt_Message pwrMsg;
	pwrMsg.component = DRV_PWR;
	pwrMsg.magnitude = power;

	PwrMgmt_Message yawMsg;
	pwrMsg.component = DRV_YAW;
	pwrMsg.magnitude = yaw;

	// Opening I2C socket
	I2C_Handle s;
	I2C_Params params;
	I2C_Params_init(&params);
	params.transferMode = I2C_MODE_BLOCKING;
	s = I2C_open(pwrInstance->i2cPeriphIndex, &params);
	if (!s)
	{
		return -1;
	}

	// Transferring data
	I2C_Transaction pwrTransaction;
	pwrTransaction.writeBuf = pwrMsg.b8;
	pwrTransaction.writeCount = 2;
	pwrTransaction.readCount = 0;
	pwrTransaction.slaveAddress = pwrInstance->pwrBoardAddr;
	if (!I2C_transfer(s, &pwrTransaction))
	{
		I2C_close(s);
		return -2;
	}

	I2C_Transaction yawTransaction;
	yawTransaction.writeBuf = yawMsg.b8;
	yawTransaction.writeCount = 2;
	yawTransaction.readCount = 0;
	yawTransaction.slaveAddress = pwrInstance->pwrBoardAddr;
	if (!I2C_transfer(s, &yawTransaction))
	{
		I2C_close(s);
		return  -2;
	}
	System_printf("Driving\n");
	System_flush();

	return 0;
}

int PwrMgmt_weapon(PwrMgmt_SvcHandle* pwrInstance, PwrMgmt_Weapon weapon, unsigned int state)
{
	// Generate transaction messages
	PwrMgmt_Message weaponMsg;
	weaponMsg.component = weapon;
	weaponMsg.magnitude = state;

	// Opening I2C sockets
	I2C_Handle s;
	I2C_Params params;
	I2C_Params_init(&params);
	params.transferMode = I2C_MODE_BLOCKING;
	s = I2C_open(pwrInstance->i2cPeriphIndex, &params);
	if (!s)
	{
		return -1;
	}

	// Transferring data
	I2C_Transaction weaponTransaction;
	weaponTransaction.writeBuf = weaponMsg.b8;
	weaponTransaction.writeCount = 2;
	weaponTransaction.readCount = 0;
	weaponTransaction.slaveAddress = pwrInstance->pwrBoardAddr;
	if (!I2C_transfer(s, &weaponTransaction))
	{
		I2C_close(s);
		return -2;
	}
	return 0;
}

int PwrMgmt_batteryRemaining(PwrMgmt_SvcHandle* pwrInstance)
{
	// Generate transaction messages
	PwrMgmt_Message batteryMsg;
	batteryMsg.component = BATTERY_REQUEST_CODE;

	// Opening I2C sockets
	I2C_Handle s;
	I2C_Params params;
	I2C_Params_init(&params);
	params.transferMode = I2C_MODE_BLOCKING;
	s = I2C_open(pwrInstance->i2cPeriphIndex, &params);
	if (!s)
	{
		return -1;
	}

	// Transferring data
	UChar batteryRemaining;

	I2C_Transaction batteryTransaction;
	batteryTransaction.writeBuf = batteryMsg.b8;
	batteryTransaction.writeCount = 1;
	batteryTransaction.readBuf = &batteryRemaining;
	batteryTransaction.readCount = 1;
	batteryTransaction.slaveAddress = pwrInstance->pwrBoardAddr;
	if (!I2C_transfer(s, &batteryTransaction))
	{
		I2C_close(s);
		return -2;
	}
	return batteryRemaining;
}
