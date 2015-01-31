/*
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

// TODO: Debug only, remove
#include <ti/drivers/I2C.h>

/* Board Header file */
#include "Board.h"

/* Killalot Framework header files */
#include "BtStack.h"
#include "PwrMgmt.h"

#include "BtCtl.h"

/*
 *  ======== main ========
 */
Int main(Void)
{
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();
    Board_initI2C();
    // Board_initDMA();
    // Board_initSPI();
    // Board_initUSB(Board_USBDEVICE);
    // Board_initWatchdog();
    // Board_initWiFi();

    /* Setup services */
    BtStack_SvcHandle mainBtSvc;
    BtStack_handleInit(&mainBtSvc, "mainBt\0", Board_BT2, BTBAUD_9600);

    PwrMgmt_SvcHandle mainPwrSvc;
    PwrMgmt_handleInit(&mainPwrSvc, "mainPwr\0", Board_INTER, 0x02);

    /* Setup apps */
    BtCtl_AppHandle mainBtApp;
    BtCtl_handleInit(&mainBtApp, "btCtl\0", &mainBtSvc, &mainPwrSvc, 0x01);


    /* Start services */
    int btStackStatus = BtStack_start(&mainBtSvc);
    if (btStackStatus == 0)
    {
    	System_printf("BtStack started successfully\n");
    	System_flush();
    }
    else if (btStackStatus == -1)
    {
    	System_printf("BtStack failed to start: task error\n");
    	System_flush();
    }

    int pwrMgmtStatus = PwrMgmt_start(&mainPwrSvc);
    if (pwrMgmtStatus == 0)
    {
    	System_printf("PwrMgmt started successfully\n");
    	System_flush();
    }
    else if (pwrMgmtStatus == -1)
    {
    	System_printf("PwrMgmt failed to start\n");
    	System_flush();
    }

    /* Start apps */
    int btCtlStatus = BtCtl_start(&mainBtApp);
    if (btCtlStatus == 0)
    {
    	System_printf("BtCtl started successfully\n");
    	System_flush();
    }
    else if (pwrMgmtStatus == -1)
    {
    	System_printf("BtCtl failed to start\n");
    }

    /* Start BIOS */
    BIOS_start();

    return (0);
}
