#pragma once

#include <furi.h>
#include <furi_hal.h>

typedef enum {
    JamModeBasic,
    JamModeEnhancedBasic,
    JamModeSweep,
    JamModeRandom,
    JamModeEmpty,
    JamModeMax
} JamMode;

typedef struct IrJammerWorker IrJammerWorker;

IrJammerWorker* ir_jammer_worker_alloc();

void ir_jammer_worker_free(IrJammerWorker* jammer);

void ir_jammer_worker_start(IrJammerWorker* jammer);

void ir_jammer_worker_stop(IrJammerWorker* jammer);

void ir_jammer_worker_set_freq(IrJammerWorker* jammer, uint32_t freq);

void ir_jammer_worker_set_mode(IrJammerWorker* jammer, JamMode mode);

void ir_jammer_worker_set_basic_timing(IrJammerWorker* jammer, uint16_t timing);

void ir_jammer_worker_set_enhanced_timing(IrJammerWorker* jammer, uint16_t mark, uint16_t space);

void ir_jammer_worker_set_sweep_timing(IrJammerWorker* jammer, uint16_t min_timing, uint16_t max_timing, uint8_t speed);

void ir_jammer_worker_set_density(IrJammerWorker* jammer, uint8_t density);

uint32_t ir_jammer_worker_get_jam_count(IrJammerWorker* jammer);

