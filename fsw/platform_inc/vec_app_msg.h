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
 *
 * Define SAMPLE App  Messages and info
 */

#ifndef VEC_APP_MSG_H
#define VEC_APP_MSG_H

/*
** SAMPLE App command codes
*/
#define VEC_APP_NOOP_CC           0
#define VEC_APP_RESET_COUNTERS_CC 1
#define VEC_APP_PROCESS_CC        2

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
} VEC_APP_NoArgsCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef VEC_APP_NoArgsCmd_t VEC_APP_NoopCmd_t;
typedef VEC_APP_NoArgsCmd_t VEC_APP_ResetCountersCmd_t;
typedef VEC_APP_NoArgsCmd_t VEC_APP_ProcessCmd_t;

/*************************************************************************/
/*
** Type definition (SAMPLE App housekeeping)
*/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    uint8 spare[2];
} VEC_APP_HkTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t  TlmHeader; /**< \brief Telemetry header */
    VEC_APP_HkTlm_Payload_t Payload;   /**< \brief Telemetry payload */
} VEC_APP_HkTlm_t;

typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    uint32 x;
    uint32 y;
    uint32 z;
} VEC_APP_Vector3_t;


#endif /* VEC_APP_MSG_H */
