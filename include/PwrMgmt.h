/**
 * \file PwrMgmt.h
 * \brief Declares power management service functions
 * \author George Xian
 * \version 0.1
 * \date 2014-12-06
 */

#ifndef PWR_MGMT
#define PWR_MGMT

typedef enum {WHEEL_LEFT = 201, WHEEL_RIGHT = 202} PwrMgmt_Wheel;
typedef enum {WEAPON_1 = 121, WEAPON_2 = 122} PwrMgmt_Weapon;

/**

 * \brief Starts power management service
 *
 * \return Returns 0 for success, -1 if service already started
 */
int8_t PwrMgmt_start(void);

/**
 * \brief Stops power management service
 *
 * \return Returns 0 for success, -1 if service failed to stop
 */
int8_t PwrMgmt_stop(void);

/**
 * \brief Returns whether service has started
 *
 * \return Flag indicating whether service has started
 */
Bool PwrMgmt_hasStarted(void);

/**
 * \brief Commands power board to drive vehicle
 *
 * \param power Forward power
 * \param yaw Yaw rate
 * \return Returns 0 for success, -1 if bus busy, -2 if address error, -3 if data error, -4 if arbitration error
 */
int8_t PwrMgmt_drive(int8_t power, int8_t yaw);

/**
 * \brief Commands power board to actuate or retract a weapon
 *
 * \param weapon ID of the weapon to actuate
 * \param state Index of the weapon state
 * \return Returns 0 for success, -1 if bus busy, -2 if address error, -3 if data error, -4 if arbitration error
 */
int8_t PwrMgmt_weapon(Weapon weapon, uint8_t state);

/**
 * \brief Request power boarxd to return the estimated remaining power
 *
 * \return Percentage of battery remaining, -1 if feature not configured
 */
int32_t PwrMgmt_batteryRemaining(void);


#endif
