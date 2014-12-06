/**
 * \file PwrMgmt.c
 * \brief Implements power management service
 * \author George Xian
 * \version 0.1
 * \date 2014-12-06
 */

#include "pwrMgmt.h"

#define DEFAULT_PWRBOARD_ADDR 0x02
typedef enum {DRV_PWR = 101, DRV_YAW = 102} DrvComponent;

static Bool hasStart = FALSE;
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

int8_t PwrMgmt_start(void)
{

}
