#include "../ir_jammer.h"
#include "../helpers/ir_jammer_worker.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct IrJammerScene1 {
    View* view;
    IrJammerScene1Callback callback;
    void* context;
};

// Common IR remote control frequencies in Hz
const uint32_t IR_FREQUENCIES[] = {30000, 33000, 36000, 38000, 40000, 42000, 56000};
const int NUM_FREQS = sizeof(IR_FREQUENCIES) / sizeof(IR_FREQUENCIES[0]);
const char* IR_MODE_NAMES[] = {"BASIC", "ENHANCED", "SWEEP", "RANDOM", "EMPTY"};

typedef struct {
    IrJammerWorker* worker;
    bool jamming_active;
    uint8_t current_freq_idx;
    JamMode current_mode;
    uint8_t setting_index;

    // new boolean to overlay info
    bool show_info;
    int info_scroll;

    uint16_t markTiming;
    uint16_t spaceTiming;
    uint16_t minTiming;
    uint16_t maxTiming;
    uint8_t sweepSpeed;
    uint8_t jamDensity;
} IrJammerScene1Model;

void ir_jammer_scene_1_set_callback(
    IrJammerScene1* instance,
    IrJammerScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

static void ir_jammer_scene_1_update_worker(IrJammerScene1Model* model) {
    if(!model->worker) return;
    ir_jammer_worker_set_freq(model->worker, IR_FREQUENCIES[model->current_freq_idx]);
    ir_jammer_worker_set_mode(model->worker, model->current_mode);
    ir_jammer_worker_set_basic_timing(model->worker, model->markTiming);
    ir_jammer_worker_set_enhanced_timing(model->worker, model->markTiming, model->spaceTiming);
    ir_jammer_worker_set_sweep_timing(
        model->worker, model->minTiming, model->maxTiming, model->sweepSpeed);
    ir_jammer_worker_set_density(model->worker, model->jamDensity);

    if(model->jamming_active) {
        ir_jammer_worker_start(model->worker);
    } else {
        ir_jammer_worker_stop(model->worker);
    }
}

void ir_jammer_scene_1_draw(Canvas* canvas, IrJammerScene1Model* model) {
    canvas_clear(canvas);

    // If info is toggled, draw the info overlay instead of the main menu
    if(model->show_info) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas, 64, 0 - model->info_scroll, AlignCenter, AlignTop, "Modes Info");

        canvas_set_font(canvas, FontSecondary);
        int y_ofs = 12 - model->info_scroll;

        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "BASIC:");
        y_ofs += 10;
        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "Common fixed marks.");
        y_ofs += 10;
        canvas_draw_str_aligned(
            canvas, 2, y_ofs, AlignLeft, AlignTop, "Reliable for most systems.");
        y_ofs += 16;

        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "ENHANCED:");
        y_ofs += 10;
        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "Tweakable mark+space.");
        y_ofs += 10;
        canvas_draw_str_aligned(
            canvas, 2, y_ofs, AlignLeft, AlignTop, "Use to fine-tune targets.");
        y_ofs += 16;

        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "SWEEP:");
        y_ofs += 10;
        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "Shifts slowly between");
        y_ofs += 10;
        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "bounds to break sync.");
        y_ofs += 16;

        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "RANDOM:");
        y_ofs += 10;
        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "Continuous random pulses");
        y_ofs += 10;
        canvas_draw_str_aligned(
            canvas, 2, y_ofs, AlignLeft, AlignTop, "to fool smart ACs and TVs.");
        y_ofs += 16;

        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "EMPTY:");
        y_ofs += 10;
        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "Uses minimal gaps;");
        y_ofs += 10;
        canvas_draw_str_aligned(
            canvas, 2, y_ofs, AlignLeft, AlignTop, "efficient but experimental.");
        y_ofs += 16;

        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "DENSITY:");
        y_ofs += 10;
        canvas_draw_str_aligned(canvas, 2, y_ofs, AlignLeft, AlignTop, "Controls how close");
        y_ofs += 10;
        canvas_draw_str_aligned(
            canvas, 2, y_ofs, AlignLeft, AlignTop, "jamming signals are packed");
        y_ofs += 10;
        canvas_draw_str_aligned(
            canvas, 2, y_ofs, AlignLeft, AlignTop, "together (higher = more logs).");

        return;
    }

    char buffer[32];
    canvas_set_font(canvas, FontSecondary);

    // Everything is moved up to save space without the title

    // Status
    snprintf(buffer, sizeof(buffer), "Status: %s", model->jamming_active ? "ACTIVE" : "PAUSED");
    if(model->setting_index == 0) canvas_draw_box(canvas, 2, 8, 124, 11);
    canvas_set_color(canvas, model->setting_index == 0 ? ColorWhite : ColorBlack);
    canvas_draw_str_aligned(canvas, 5, 10, AlignLeft, AlignTop, buffer);
    canvas_set_color(canvas, ColorBlack);

    // Freq
    snprintf(
        buffer, sizeof(buffer), "Freq: %lu kHz", IR_FREQUENCIES[model->current_freq_idx] / 1000);
    if(model->setting_index == 1) canvas_draw_box(canvas, 2, 18, 124, 11);
    canvas_set_color(canvas, model->setting_index == 1 ? ColorWhite : ColorBlack);
    canvas_draw_str_aligned(canvas, 5, 20, AlignLeft, AlignTop, buffer);
    canvas_set_color(canvas, ColorBlack);

    // Mode
    snprintf(buffer, sizeof(buffer), "Mode: %s", IR_MODE_NAMES[model->current_mode]);
    if(model->setting_index == 2) canvas_draw_box(canvas, 2, 28, 124, 11);
    canvas_set_color(canvas, model->setting_index == 2 ? ColorWhite : ColorBlack);
    canvas_draw_str_aligned(canvas, 5, 30, AlignLeft, AlignTop, buffer);
    canvas_set_color(canvas, ColorBlack);

    // Density
    snprintf(buffer, sizeof(buffer), "Density: %d", model->jamDensity);
    if(model->setting_index == 3) canvas_draw_box(canvas, 2, 38, 124, 11);
    canvas_set_color(canvas, model->setting_index == 3 ? ColorWhite : ColorBlack);
    canvas_draw_str_aligned(canvas, 5, 40, AlignLeft, AlignTop, buffer);
    canvas_set_color(canvas, ColorBlack);

    // Info
    snprintf(buffer, sizeof(buffer), "Info");
    if(model->setting_index == 4) canvas_draw_box(canvas, 2, 48, 124, 11);
    canvas_set_color(canvas, model->setting_index == 4 ? ColorWhite : ColorBlack);
    canvas_draw_str_aligned(canvas, 5, 50, AlignLeft, AlignTop, buffer);
    canvas_set_color(canvas, ColorBlack);

    // Jams
    if(model->worker) {
        snprintf(buffer, sizeof(buffer), "J: %lu", ir_jammer_worker_get_jam_count(model->worker));
        canvas_draw_str_aligned(canvas, 100, 50, AlignLeft, AlignTop, buffer);
    }
}

static void ir_jammer_scene_1_model_init(IrJammerScene1Model* const model) {
    model->jamming_active = false;
    model->current_freq_idx = 3;
    model->current_mode = JamModeBasic;
    model->setting_index = 0;
    model->show_info = false;
    model->info_scroll = 0;
    model->markTiming = 12;
    model->spaceTiming = 12;
    model->minTiming = 8;
    model->maxTiming = 70;
    model->sweepSpeed = 1;
    model->jamDensity = 5;
    if(!model->worker) {
        model->worker = ir_jammer_worker_alloc();
        ir_jammer_scene_1_update_worker(model);
    }
}

bool ir_jammer_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    IrJammerScene1* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                IrJammerScene1Model * model,
                {
                    if(model->show_info) {
                        model->show_info = false;
                        model->info_scroll = 0;
                    } else {
                        if(model->worker) {
                            ir_jammer_worker_stop(model->worker);
                        }
                        instance->callback(IrJammerCustomEventScene1Back, instance->context);
                    }
                },
                true);
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                IrJammerScene1Model * model,
                {
                    if(!model->show_info) {
                        model->setting_index =
                            (model->setting_index > 0) ? (model->setting_index - 1) : 4;
                    } else {
                        if(model->info_scroll > 0) model->info_scroll -= 8;
                    }
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                IrJammerScene1Model * model,
                {
                    if(!model->show_info) {
                        model->setting_index =
                            (model->setting_index < 4) ? (model->setting_index + 1) : 0;
                    } else {
                        if(model->info_scroll < 180) model->info_scroll += 8;
                    }
                },
                true);
            break;
        case InputKeyLeft:
        case InputKeyRight:
            with_view_model(
                instance->view,
                IrJammerScene1Model * model,
                {
                    if(!model->show_info) {
                        int dir = (event->key == InputKeyRight) ? 1 : -1;
                        switch(model->setting_index) {
                        case 0:
                            model->jamming_active = !model->jamming_active;
                            break;
                        case 1:
                            model->current_freq_idx =
                                (model->current_freq_idx + dir + NUM_FREQS) % NUM_FREQS;
                            break;
                        case 2:
                            model->current_mode =
                                (JamMode)((model->current_mode + dir + JamModeMax) % JamModeMax);
                            break;
                        case 3:
                            if(dir > 0 && model->jamDensity < 20) model->jamDensity++;
                            if(dir < 0 && model->jamDensity > 1) model->jamDensity--;
                            break;
                        case 4:
                            if(event->key == InputKeyRight) {
                                model->show_info = true;
                            }
                            break;
                        }
                        ir_jammer_scene_1_update_worker(model);
                    }
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                IrJammerScene1Model * model,
                {
                    if(model->show_info) {
                        model->show_info = false;
                        model->info_scroll = 0;
                    } else if(model->setting_index == 4) {
                        model->show_info = true;
                    } else {
                        model->jamming_active = !model->jamming_active;
                        ir_jammer_scene_1_update_worker(model);
                    }
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void ir_jammer_scene_1_exit(void* context) {
    furi_assert(context);
    IrJammerScene1* instance = (IrJammerScene1*)context;
    with_view_model(
        instance->view,
        IrJammerScene1Model * model,
        {
            if(model->worker) {
                ir_jammer_worker_free(model->worker);
                model->worker = NULL;
            }
        },
        true);
}

void ir_jammer_scene_1_enter(void* context) {
    furi_assert(context);
    IrJammerScene1* instance = (IrJammerScene1*)context;
    with_view_model(
        instance->view,
        IrJammerScene1Model * model,
        { ir_jammer_scene_1_model_init(model); },
        true);
}

IrJammerScene1* ir_jammer_scene_1_alloc() {
    IrJammerScene1* instance = malloc(sizeof(IrJammerScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(IrJammerScene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)ir_jammer_scene_1_draw);
    view_set_input_callback(instance->view, ir_jammer_scene_1_input);
    view_set_enter_callback(instance->view, ir_jammer_scene_1_enter);
    view_set_exit_callback(instance->view, ir_jammer_scene_1_exit);

    return instance;
}

void ir_jammer_scene_1_free(IrJammerScene1* instance) {
    furi_assert(instance);
    with_view_model(
        instance->view,
        IrJammerScene1Model * model,
        {
            if(model->worker) {
                ir_jammer_worker_free(model->worker);
            }
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* ir_jammer_scene_1_get_view(IrJammerScene1* instance) {
    furi_assert(instance);
    return instance->view;
}
