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

#ifndef SEND_VECTOR3_APP_H
#define SEND_VECTOR3_APP_H

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "send_vector3_app_perfids.h"
#include "send_vector3_app_msgids.h"
#include "send_vector3_app_msg.h"

/***********************************************************************/
#define SEND_VECTOR3_APP_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */
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
    SEND_VECTOR3_APP_HkTlm_t HkTlm;

    /*
    ** RandAssNum telemetry packet
    */
    SEND_VECTOR3_APP_RandAssNum_t RndAssNum;

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

    CFE_EVS_BinFilter_t EventFilters[SEND_VECTOR3_APP_EVENT_COUNTS];

} SEND_VECTOR3_APP_Data_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (SEND_VECTOR3_APP_Main), these
**       functions are not called from any other source module.
*/
void  SEND_VECTOR3_APP_Main(void);
int32 SEND_VECTOR3_APP_Init(void);
int32 SEND_VECTOR3_APP_ResetCounters(const SEND_VECTOR3_APP_ResetCountersCmd_t *Msg);
int32 SEND_VECTOR3_APP_Process(const SEND_VECTOR3_APP_ProcessCmd_t *Msg);
int32 SEND_VECTOR3_APP_Noop(const SEND_VECTOR3_APP_NoopCmd_t *Msg);

#endif /* SEND_VECTOR3_APP_H */
