#include "../ir_jammer.h"
#include "../helpers/ir_jammer_custom_event.h"
#include "../views/ir_jammer_scene_1.h"

void ir_jammer_scene_1_callback(IrJammerCustomEvent event, void* context) {
    furi_assert(context);
    IrJammer* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void ir_jammer_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    IrJammer* app = context;
    ir_jammer_scene_1_set_callback(app->ir_jammer_scene_1, ir_jammer_scene_1_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, IrJammerViewIdScene1);
}

bool ir_jammer_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    IrJammer* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case IrJammerCustomEventScene1Left:
        case IrJammerCustomEventScene1Right:
            break;
        case IrJammerCustomEventScene1Up:
        case IrJammerCustomEventScene1Down:
            break;
        case IrJammerCustomEventScene1Back:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            scene_manager_stop(app->scene_manager);
            view_dispatcher_stop(app->view_dispatcher);
            consumed = true;
            break;
        }
    }

    return consumed;
}

void ir_jammer_scene_scene_1_on_exit(void* context) {
    IrJammer* app = context;
    UNUSED(app);
}