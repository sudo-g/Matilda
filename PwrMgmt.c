/**
 * \file PwrMgmt.c
 * \brief Implements power management service
 * \author George Xian
 * \version 0.1
 * \date 2014-12-06
 */

#include "pwrMgmt.h"

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/drivers/I2C.h>

#include "Board.h"

#define DEFAULT_PWRBOARD_ADDR 0x02
#define BATTERY_REQUEST_CODE 131
typedef enum {DRV_PWR = 101, DRV_YAW = 102} DrvComponent;

static uint8_t pwrBoardAddr = DEFAULT_PWRBOARD_ADDR;

typedef union
{
	struct
	{
		int8_t component;
		int8_t magnitude;
	};
	UChar b8[2];
} PwrMgmt_Message;

int8_t PwrMgmt_drive(int8_t power, int8_t yaw)
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
	s = I2C_open(Board_INTER, &params);
	if (!s)
	{
		return -1;
	}

	// Transferring data
	I2C_Transaction pwrTransaction;
	pwrTransaction.writeBuf = pwrMsg.b8;
	pwrTransaction.writeCount = 2;
	pwrTransaction.readCount = 0;
	pwrTransaction.slaveAddress = pwrBoardAddr;
	if (!I2C_transfer(s, &pwrTransaction))
	{
		I2C_close(s);
		return -2;
	}

	I2C_Transaction yawTransaction;
	yawTransaction.writeBuf = yawMsg.b8;
	yawTransaction.writeCount = 2;
	yawTransaction.readCount = 0;
	yawTransaction.slaveAddress = pwrBoardAddr;
	if (!I2C_transfer(s, &yawTransaction))
	{
		I2C_close(s);
		return  -2;
	}

	return 0;
}

int8_t PwrMgmt_weapon(PwrMgmt_Weapon weapon, uint8_t state)
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
	s = I2C_open(Board_INTER, &params);
	if (!s)
	{
		return -1;
	}

	// Transferring data
	I2C_Transaction weaponTransaction;
	weaponTransaction.writeBuf = weaponMsg.b8;
	weaponTransaction.writeCount = 2;
	weaponTransaction.readCount = 0;
	weaponTransaction.slaveAddress = pwrBoardAddr;
	if (!I2C_transfer(s, &weaponTransaction))
	{
		I2C_close(s);
		return -2;
	}
	return 0;
}

int8_t PwrMgmt_batteryRemaining(void)
{
	// Generate transaction messages
	PwrMgmt_Message batteryMsg;
	batteryMsg.component = BATTERY_REQUEST_CODE;

	// Opening I2C sockets
	I2C_Handle s;
	I2C_Params params;
	I2C_Params_init(&params);
	params.transferMode = I2C_MODE_BLOCKING;
	s = I2C_open(Board_INTER, &params);
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
	batteryTransaction.slaveAddress = pwrBoardAddr;
	if (!I2C_transfer(s, &batteryTransaction))
	{
		I2C_close(s);
		return -2;
	}
	return batteryRemaining;
}
