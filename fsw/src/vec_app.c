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
#include "ros_app_msg.h"

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
    uint32            status;
    CFE_SB_Buffer_t *SBBufPtr;

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
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(VEC_APP_PERF_ID);

        /*
        ** Get current time
        */
        CFE_TIME_SysTime_t newtime = CFE_TIME_GetTime();
        if ((newtime.Seconds - time.Seconds) >= 2 && counter <= 10)
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

            CFE_ES_WriteToSysLog("VEC_APP: Sent Vector3 packet #%d to software bus.\n",counter);

            counter ++;
            // if (counter > 10){ VEC_APP_Data.RunStatus = CFE_ES_RunStatus_APP_EXIT; }
        }

        /* Pend on receipt of command packet */
        // CFE_ES_WriteToSysLog("VEC_APP: Polling command pipe for messages.\n");
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, VEC_APP_Data.CommandPipe, CFE_SB_POLL);
        // CFE_ES_WriteToSysLog("VEC_APP: Finished polling command pipe for messages.\n");
        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(VEC_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            VEC_APP_ProcessCommandPacket(SBBufPtr);
        }
        else if (status == CFE_SB_NO_MESSAGE)
        {
            //do nothing
        }
        else
        {
            CFE_EVS_SendEvent(VEC_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                                "VEC_APP: SB Pipe Read Error, App Will Exit. Error Code is - %d",status);

            VEC_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
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
        CFE_ES_WriteToSysLog("VEC_APP: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
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
        CFE_ES_WriteToSysLog("VEC_APP: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to vector packets coming from ROS_APP
    */
    status = CFE_SB_Subscribe(VEC_APP_ROSVEC3_MID, VEC_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("VEC_APP: Error Subscribing to Vector request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    CFE_EVS_SendEvent(VEC_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "Vec App Initialized.%s",
                      VEC_APP_VERSION_STRING);

    return (CFE_SUCCESS);

} /* End of VEC_APP_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  VEC_APP_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the VEC    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void VEC_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (MsgId)
    {
        case VEC_APP_ROSVEC3_MID:
            VEC_APP_ProcessVecMsg(SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(VEC_APP_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "ROS: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End VEC_APP_ProcessCommandPacket */

void VEC_APP_ProcessVecMsg(CFE_SB_Buffer_t *SBBufPtr)
{
    if (VEC_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(ROS_APP_Vector3_t)))
    {
        ROS_APP_Vector3_t *vecMsg = (ROS_APP_Vector3_t *)(&(SBBufPtr->Msg));
        double x = vecMsg->x;
        double y = vecMsg->y;
        double z = vecMsg->z;
        CFE_ES_WriteToSysLog("VEC_APP: Received Vec Msg from software bus: %.0f, %.0f, %.0f\n", x, y, z);
    }
}

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* ROS_APP_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool VEC_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(VEC_APP_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        VEC_APP_Data.ErrCounter++;
    }

    return (result);

} /* End of ROS_APP_VerifyCmdLength() */
