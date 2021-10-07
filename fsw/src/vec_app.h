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
*******************************************************************************/

/**
 * @file
 * @author Edit
 * Main header file for the SAMPLE application
 */

#ifndef VEC_APP_H
#define VEC_APP_H

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "vec_app_perfids.h"
#include "vec_app_msgids.h"
#include "vec_app_msg.h"

/***********************************************************************/
#define VEC_APP_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */
/************************************************************************
** Type Definitions
*************************************************************************/

/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8 CmdCounter;
    uint8 ErrCounter;

    /*
    ** Housekeeping telemetry packet...
    */
    VEC_APP_HkTlm_t HkTlm;

    /*
    ** RandAssNum telemetry packet
    */
    VEC_APP_Vector3_t Vector3;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CommandPipe;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char   PipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    CFE_EVS_BinFilter_t EventFilters[VEC_APP_EVENT_COUNTS];

} VEC_APP_Data_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (VEC_APP_Main), these
**       functions are not called from any other source module.
*/
void  VEC_APP_Main(void);
int32 VEC_APP_Init(void);
int32 VEC_APP_ResetCounters(const VEC_APP_ResetCountersCmd_t *Msg);
int32 VEC_APP_Process(const VEC_APP_ProcessCmd_t *Msg);
int32 VEC_APP_Noop(const VEC_APP_NoopCmd_t *Msg);
void VEC_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void VEC_APP_ProcessVecMsg(CFE_SB_Buffer_t *SBBufPtr);
bool VEC_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

#endif /* VEC_APP_H */
