#include "ir_jammer.h"

bool ir_jammer_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    IrJammer* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void ir_jammer_tick_event_callback(void* context) {
    furi_assert(context);
    IrJammer* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool ir_jammer_navigation_event_callback(void* context) {
    furi_assert(context);
    IrJammer* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

IrJammer* ir_jammer_app_alloc() {
    IrJammer* app = malloc(sizeof(IrJammer));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();

    app->scene_manager = scene_manager_alloc(&ir_jammer_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, ir_jammer_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, ir_jammer_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, ir_jammer_custom_event_callback);

    app->ir_jammer_scene_1 = ir_jammer_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        IrJammerViewIdScene1,
        ir_jammer_scene_1_get_view(app->ir_jammer_scene_1));

    //End Scene Additions

    return app;
}

void ir_jammer_app_free(IrJammer* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, IrJammerViewIdScene1);
    ir_jammer_scene_1_free(app->ir_jammer_scene_1);

    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    app->gui = NULL;
    app->notification = NULL;

    //Remove whatever is left
    free(app);
}

int32_t ir_jammer_app(void* p) {
    UNUSED(p);
    IrJammer* app = ir_jammer_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, IrJammerSceneScene_1); //Start with IR Jammer

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();
    ir_jammer_app_free(app);

    return 0;
}
