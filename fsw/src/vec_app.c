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
** File: vec_app.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "vec_app_events.h"
#include "vec_app_version.h"
#include "vec_app.h"

/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>

/*
** global data
*/
VEC_APP_Data_t VEC_APP_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* VEC_APP_Main() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void VEC_APP_Main(void)
{
    int32            status;
    // CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(VEC_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = VEC_APP_Init();
    if (status != CFE_SUCCESS)
    {
        VEC_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }


    /*
    ** Variable to hold time and initialize loop counter
    */
    CFE_TIME_SysTime_t time;
    time.Seconds = 0;
    time.Subseconds = 0;
    uint32 counter = 0;

    /*
    ** VEC Runloop
    */
    while (CFE_ES_RunLoop(&VEC_APP_Data.RunStatus) == true)
    {
        if (counter > 10){ break; }

        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(VEC_APP_PERF_ID);

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
            VEC_APP_Data.Vector3.CommandErrorCounter = VEC_APP_Data.ErrCounter;
            VEC_APP_Data.Vector3.CommandCounter      = VEC_APP_Data.CmdCounter;

            VEC_APP_Data.Vector3.x = time.Seconds;
            VEC_APP_Data.Vector3.y = time.Subseconds;
            VEC_APP_Data.Vector3.z = counter;

            CFE_SB_TimeStampMsg(&VEC_APP_Data.Vector3.TlmHeader.Msg);
            CFE_SB_TransmitMsg(&VEC_APP_Data.Vector3.TlmHeader.Msg, true);

            CFE_ES_WriteToSysLog("Sent Vector3 packet #%d\n",counter);

            counter ++;
        }

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(VEC_APP_PERF_ID);
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(VEC_APP_PERF_ID);

    CFE_ES_ExitApp(VEC_APP_Data.RunStatus);

} /* End of VEC_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* VEC_APP_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 VEC_APP_Init(void)
{
    int32 status;

    VEC_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    VEC_APP_Data.CmdCounter = 0;
    VEC_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    VEC_APP_Data.PipeDepth = VEC_APP_PIPE_DEPTH;

    strncpy(VEC_APP_Data.PipeName, "VEC_APP_CMD_PIPE", sizeof(VEC_APP_Data.PipeName));
    VEC_APP_Data.PipeName[sizeof(VEC_APP_Data.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    VEC_APP_Data.EventFilters[0].EventID = VEC_APP_STARTUP_INF_EID;
    VEC_APP_Data.EventFilters[0].Mask    = 0x0000;
    VEC_APP_Data.EventFilters[1].EventID = VEC_APP_COMMAND_ERR_EID;
    VEC_APP_Data.EventFilters[1].Mask    = 0x0000;
    VEC_APP_Data.EventFilters[2].EventID = VEC_APP_COMMANDNOP_INF_EID;
    VEC_APP_Data.EventFilters[2].Mask    = 0x0000;
    VEC_APP_Data.EventFilters[3].EventID = VEC_APP_COMMANDRST_INF_EID;
    VEC_APP_Data.EventFilters[3].Mask    = 0x0000;
    VEC_APP_Data.EventFilters[4].EventID = VEC_APP_INVALID_MSGID_ERR_EID;
    VEC_APP_Data.EventFilters[4].Mask    = 0x0000;
    VEC_APP_Data.EventFilters[5].EventID = VEC_APP_LEN_ERR_EID;
    VEC_APP_Data.EventFilters[5].Mask    = 0x0000;
    VEC_APP_Data.EventFilters[6].EventID = VEC_APP_PIPE_ERR_EID;
    VEC_APP_Data.EventFilters[6].Mask    = 0x0000;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(VEC_APP_Data.EventFilters, VEC_APP_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Send Vec App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize vector packet (clear user data area).
    */
    CFE_MSG_Init(&VEC_APP_Data.Vector3.TlmHeader.Msg, VEC_APP_VECTOR3_MID, sizeof(VEC_APP_Data.Vector3));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&VEC_APP_Data.CommandPipe, VEC_APP_Data.PipeDepth, VEC_APP_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Send Vec App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    CFE_EVS_SendEvent(VEC_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "Vec App Initialized.%s",
                      VEC_APP_VERSION_STRING);

    return (CFE_SUCCESS);

} /* End of VEC_APP_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* VEC_APP_Noop -- VEC NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 VEC_APP_Noop(const VEC_APP_NoopCmd_t *Msg)
{

    VEC_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(VEC_APP_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "Vec App: NOOP command %s",
                      VEC_APP_VERSION);

    return CFE_SUCCESS;

} /* End of VEC_APP_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  VEC_APP_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 VEC_APP_ResetCounters(const VEC_APP_ResetCountersCmd_t *Msg)
{

    VEC_APP_Data.CmdCounter = 0;
    VEC_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(VEC_APP_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "Vec App: RESET command");

    return CFE_SUCCESS;

} /* End of VEC_APP_ResetCounters() */
