//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Esp32_os.h>
#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <target_platform.h>
#include <nanoPAL_BlockStorage.h>


//
//  Reboot hnadlers clean up on reboot
//
static ON_SOFT_REBOOT_HANDLER s_rebootHandlers[16] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void HAL_AddSoftRebootHandler(ON_SOFT_REBOOT_HANDLER handler)
{
    for(int i=0; i<ARRAYSIZE(s_rebootHandlers); i++)
    {
        if(s_rebootHandlers[i] == NULL)
        {
            s_rebootHandlers[i] = handler;
            return;
        }
        else if(s_rebootHandlers[i] == handler)
        {
            return;
        }
    }
}

// because nanoHAL_Initialize needs to be called in both C and C++ we need a proxy to allow it to be called in 'C'
extern "C" {
    
    void nanoHAL_Initialize_C()
    {
        nanoHAL_Initialize();
    }
}

void nanoHAL_Initialize()
{
    HAL_CONTINUATION::InitializeList();
    HAL_COMPLETION  ::InitializeList();

    // initialize block storage devices
    BlockStorage_AddDevices();

    // clear managed heap region
    unsigned char* heapStart = NULL;
    unsigned int heapSize  = 0;

    ::HeapLocation( heapStart, heapSize );
    memset(heapStart, 0, heapSize);

    Events_Initialize();

    // no PAL events required until now
    //PalEvent_Initialize();
}

void nanoHAL_Uninitialize()
{
    // check for s_rebootHandlers
    for(int i = 0; i< ARRAYSIZE(s_rebootHandlers); i++)
    {
        if(s_rebootHandlers[i] != NULL)
        {
            s_rebootHandlers[i]();
        }
        else
        {
            break;
        }
    }   
    
    //PalEvent_Uninitialize();

    Events_Uninitialize();
    
    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION  ::Uninitialize();
}


