#pragma once

#include <gui/view.h>
#include "../helpers/ir_jammer_custom_event.h"

typedef struct IrJammerScene1 IrJammerScene1;

typedef void (*IrJammerScene1Callback)(IrJammerCustomEvent event, void* context);

void ir_jammer_scene_1_set_callback(
    IrJammerScene1* ir_jammer_scene_1,
    IrJammerScene1Callback callback,
    void* context);

View* ir_jammer_scene_1_get_view(IrJammerScene1* ir_jammer_static);

IrJammerScene1* ir_jammer_scene_1_alloc();

void ir_jammer_scene_1_free(IrJammerScene1* ir_jammer_static);