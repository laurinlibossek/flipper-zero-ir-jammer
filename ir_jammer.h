#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include "scenes/ir_jammer_scene.h"
#include "views/ir_jammer_scene_1.h"

#define TAG "IrJammer"
#define IR_JAMMER_VERSION "1.0"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    IrJammerScene1* ir_jammer_scene_1;
} IrJammer;

typedef enum {
    IrJammerViewIdScene1,
} IrJammerViewId;
