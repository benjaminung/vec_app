/*******************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: send_vector3_app.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "send_vector3_app_events.h"
#include "send_vector3_app_version.h"
#include "send_vector3_app.h"

/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>
#include "sample_lib.h"

/*
** global data
*/
SEND_VECTOR3_APP_Data_t SEND_VECTOR3_APP_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* SEND_VECTOR3_APP_Main() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void SEND_VECTOR3_APP_Main(void)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(SEND_VECTOR3_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = SEND_VECTOR3_APP_Init();
    if (status != CFE_SUCCESS)
    {
        SEND_VECTOR3_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }


    /*
    ** Variable to hold time and initialize loop counter
    */
    CFE_TIME_SysTime_t time;
    time.Seconds = 0;
    time.Subseconds = 0;
    uint32 counter = 0;

    /*
    ** SAMPLE Runloop
    */
    while (CFE_ES_RunLoop(&SEND_VECTOR3_APP_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(SEND_VECTOR3_APP_PERF_ID);

        /*
        ** Get current time
        */
        CFE_TIME_SysTime_t newtime = CFE_TIME_GetTime();
        if ((newtime.Seconds - time.Seconds) >= 2)
        {
            time = newtime;
            /*
            ** Send random ass number
            */
            SAMPLE_APP_Data.Vector3.CommandErrorCounter = SAMPLE_APP_Data.ErrCounter;
            SAMPLE_APP_Data.Vector3.CommandCounter      = SAMPLE_APP_Data.CmdCounter;

            SAMPLE_APP_Data.Vector3.x = time.Seconds;
            SAMPLE_APP_Data.Vector3.y = time.Subseconds;
            SAMPLE_APP_Data.Vector3.z = counter;

            CFE_SB_TimeStampMsg(&SAMPLE_APP_Data.Vector3.TlmHeader.Msg);
            CFE_SB_TransmitMsg(&SAMPLE_APP_Data.Vector3.TlmHeader.Msg, true);

            CFE_ES_WriteToSysLog("Sent Vector3 packet #%d\n",counter);

            counter ++;
        }

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(SEND_VECTOR3_APP_PERF_ID);
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(SEND_VECTOR3_APP_PERF_ID);

    CFE_ES_ExitApp(SEND_VECTOR3_APP_Data.RunStatus);

} /* End of SEND_VECTOR3_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* SEND_VECTOR3_APP_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 SEND_VECTOR3_APP_Init(void)
{
    int32 status;

    SEND_VECTOR3_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    SEND_VECTOR3_APP_Data.CmdCounter = 0;
    SEND_VECTOR3_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    SEND_VECTOR3_APP_Data.PipeDepth = SEND_VECTOR3_APP_PIPE_DEPTH;

    strncpy(SEND_VECTOR3_APP_Data.PipeName, "SEND_VECTOR3_APP_CMD_PIPE", sizeof(SEND_VECTOR3_APP_Data.PipeName));
    SEND_VECTOR3_APP_Data.PipeName[sizeof(SEND_VECTOR3_APP_Data.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    SEND_VECTOR3_APP_Data.EventFilters[0].EventID = SEND_VECTOR3_APP_STARTUP_INF_EID;
    SEND_VECTOR3_APP_Data.EventFilters[0].Mask    = 0x0000;
    SEND_VECTOR3_APP_Data.EventFilters[1].EventID = SEND_VECTOR3_APP_COMMAND_ERR_EID;
    SEND_VECTOR3_APP_Data.EventFilters[1].Mask    = 0x0000;
    SEND_VECTOR3_APP_Data.EventFilters[2].EventID = SEND_VECTOR3_APP_COMMANDNOP_INF_EID;
    SEND_VECTOR3_APP_Data.EventFilters[2].Mask    = 0x0000;
    SEND_VECTOR3_APP_Data.EventFilters[3].EventID = SEND_VECTOR3_APP_COMMANDRST_INF_EID;
    SEND_VECTOR3_APP_Data.EventFilters[3].Mask    = 0x0000;
    SEND_VECTOR3_APP_Data.EventFilters[4].EventID = SEND_VECTOR3_APP_INVALID_MSGID_ERR_EID;
    SEND_VECTOR3_APP_Data.EventFilters[4].Mask    = 0x0000;
    SEND_VECTOR3_APP_Data.EventFilters[5].EventID = SEND_VECTOR3_APP_LEN_ERR_EID;
    SEND_VECTOR3_APP_Data.EventFilters[5].Mask    = 0x0000;
    SEND_VECTOR3_APP_Data.EventFilters[6].EventID = SEND_VECTOR3_APP_PIPE_ERR_EID;
    SEND_VECTOR3_APP_Data.EventFilters[6].Mask    = 0x0000;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(SEND_VECTOR3_APP_Data.EventFilters, SEND_VECTOR3_APP_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(&SEND_VECTOR3_APP_Data.Vector3.TlmHeader.Msg, SEND_VECTOR3_APP_VECTOR3_MID, sizeof(SEND_VECTOR3_APP_Data.Vector3));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&SEND_VECTOR3_APP_Data.CommandPipe, SEND_VECTOR3_APP_Data.PipeDepth, SEND_VECTOR3_APP_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(SEND_VECTOR3_APP_VECTOR3_MID, SEND_VECTOR3_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Send Vector3 App: Error Subscribing to Vector3 request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    CFE_EVS_SendEvent(SEND_VECTOR3_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE App Initialized.%s",
                      SEND_VECTOR3_APP_VERSION_STRING);

    return (CFE_SUCCESS);

} /* End of SEND_VECTOR3_APP_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SEND_VECTOR3_APP_Noop -- SAMPLE NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 SEND_VECTOR3_APP_Noop(const SEND_VECTOR3_APP_NoopCmd_t *Msg)
{

    SEND_VECTOR3_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(SEND_VECTOR3_APP_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: NOOP command %s",
                      SEND_VECTOR3_APP_VERSION);

    return CFE_SUCCESS;

} /* End of SEND_VECTOR3_APP_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SEND_VECTOR3_APP_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 SEND_VECTOR3_APP_ResetCounters(const SEND_VECTOR3_APP_ResetCountersCmd_t *Msg)
{

    SEND_VECTOR3_APP_Data.CmdCounter = 0;
    SEND_VECTOR3_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(SEND_VECTOR3_APP_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: RESET command");

    return CFE_SUCCESS;

} /* End of SEND_VECTOR3_APP_ResetCounters() */
