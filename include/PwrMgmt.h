/**
 * \file PwrMgmt.h
 * \brief Declares power management service functions
 * \author George Xian
 * \version 0.1
 * \date 2014-12-06
 */

#ifndef PWR_MGMT
#define PWR_MGMT

#define PWRMGMT_SVCNAMELEN 8

#include <xdc/std.h>

typedef enum {WEAPON_1 = 121, WEAPON_2 = 122} PwrMgmt_Weapon;

/**
 * \struct PwrMgmt_SvcHandle
 * \brief Handler to an instance of a PwrMgmt app
 */
typedef struct
{
	unsigned int i2cPeriphIndex;
	unsigned int pwrBoardAddr;
	Bool started;
	char svcName[PWRMGMT_SVCNAMELEN];
} PwrMgmt_SvcHandle ;

/**
 * \brief Initializes handle for default service configuration
 *
 * \param pwrHandle
 * \param name
 * \param i2cPeriphIndex
 * \param pwrBoardAddr
 */
void PwrMgmt_handleInit(PwrMgmt_SvcHandle* pwrHandle, char* name, unsigned int i2cPeriphIndex, unsigned int pwrBoardAddr);

/**
 * \param pwrInstance Pointer to the handle containing configuration for this service
 * \return Returns 0 if sucessfully started, -1 if failed
 */
int PwrMgmt_start(PwrMgmt_SvcHandle* pwrInstance);

/**
 * \param pwrInstance Pointer to the handle containing configuration for this service
 * \return Returns 0 if successfully stopped, -1 if failed
 */
int PwrMgmt_stop(PwrMgmt_SvcHandle* pwrInstance);

/**
 * \brief Commands power board to drive vehicle
 *
 * \param pwrInstance Pointer to the handle of the service instance to use
 * \param power Forward power
 * \param yaw Yaw rate
 * \return Returns 0 for success, -1 if socket failed to open, -2 if transaction error
 */
int PwrMgmt_drive(PwrMgmt_SvcHandle* pwrInstance, int power, int yaw);

/**
 * \brief Commands power board to actuate or retract a weapon
 *
 * \param pwrInstance Pointer to the handle of the service instance to use
 * \param weapon ID of the weapon to actuate
 * \param state Index of the weapon state
 * \return Returns 0 for success, -1 if socket failed to open, -2 if transaction error
 */
int PwrMgmt_weapon(PwrMgmt_SvcHandle* pwrInstance, PwrMgmt_Weapon weapon, unsigned int state);

/**
 * \brief Request power boarxd to return the estimated remaining power
 *
 * \param pwrInstance Pointer to the handle of the service instance to use
 * \return Percentage of battery remaining, -1 if socket failed to open, -2 if transaction error
 */
int PwrMgmt_batteryRemaining(PwrMgmt_SvcHandle* pwrInstance);


#endif
