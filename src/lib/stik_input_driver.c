/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include "clog/clog.h"
#include <imprint/allocator.h>
#include <sense-steam/stik_input_manager.h>
#include <sense/sense_input.h>
#include <sense/sense_input_manager.h>

static void update(void* _self, SenseInput* target)
{
    SenseStikInputManager* self = (SenseStikInputManager*)_self;
    senseStikInputManagerUpdate(self, target);
}

void senseInputManagerCreatePlatformDriver(
    SenseInputManager* target, struct ImprintAllocator* allocator, BlSize2i screen_size)
{
    (void)screen_size;

    SenseStikInputManager* self = IMPRINT_ALLOC_TYPE(allocator, SenseStikInputManager);
    int result = senseStikInputManagerInit(self, g_steamApiAtheneum);
    if (result < 0) {
        CLOG_ERROR("could not initialize stik %d", result)
    }

    target->self = self;
    target->update_fn = update;
}
