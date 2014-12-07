/**
 * \file PwrMgmt.h
 * \brief Declares power management service functions
 * \author George Xian
 * \version 0.1
 * \date 2014-12-06
 */

#ifndef PWR_MGMT
#define PWR_MGMT

#include <stdint.h>

typedef enum {WEAPON_1 = 121, WEAPON_2 = 122} PwrMgmt_Weapon;

/**
 * \brief Commands power board to drive vehicle
 *
 * \param power Forward power
 * \param yaw Yaw rate
 * \return Returns 0 for success, -1 if socket failed to open, -2 if transaction error
 */
int8_t PwrMgmt_drive(int8_t power, int8_t yaw);

/**
 * \brief Commands power board to actuate or retract a weapon
 *
 * \param weapon ID of the weapon to actuate
 * \param state Index of the weapon state
 * \return Returns 0 for success, -1 if socket failed to open, -2 if transaction error
 */
int8_t PwrMgmt_weapon(PwrMgmt_Weapon weapon, uint8_t state);

/**
 * \brief Request power boarxd to return the estimated remaining power
 *
 * \return Percentage of battery remaining, -1 if socket failed to open, -2 if transaction error
 */
int8_t PwrMgmt_batteryRemaining(void);


#endif
