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

/**
 * \brief Sets drive speed according to received frame
 *
 * \param frame Bluetooth frame received
 *
 */
void BtCtl_rxCallback(const BtStack_Frame* frame);

#endif
