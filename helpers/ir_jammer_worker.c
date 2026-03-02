#include "ir_jammer_worker.h"
#include <furi_hal_infrared.h>
#include <furi_hal_light.h>

struct IrJammerWorker {
    FuriThread* thread;
    volatile bool running;
    volatile JamMode mode;
    volatile uint32_t freq;

    volatile uint16_t markTiming;
    volatile uint16_t spaceTiming;
    volatile uint16_t minTiming;
    volatile uint16_t maxTiming;
    volatile uint8_t sweepSpeed;
    volatile int8_t sweepDir;
    volatile uint8_t jamDensity;

    volatile uint32_t jamCount;

    // Internal state for ISR
    volatile uint32_t pulses_remaining;
    volatile bool current_level;
    volatile uint16_t current_mark;
    volatile uint16_t current_space;
};

static FuriHalInfraredTxGetDataState
    ir_jammer_worker_data_isr(void* context, uint32_t* duration, bool* level) {
    IrJammerWorker* worker = context;
    if(worker->pulses_remaining == 0) {
        return FuriHalInfraredTxGetDataStateLastDone;
    }

    *level = worker->current_level;
    *duration = worker->current_level ? worker->current_mark : worker->current_space;

    worker->current_level = !worker->current_level;
    worker->pulses_remaining--;
    return FuriHalInfraredTxGetDataStateOk;
}

static int32_t ir_jammer_worker_thread(void* context) {
    IrJammerWorker* worker = context;
    // ensure IR hardware is ready, tx pin is configured properly by defaults

    furi_hal_infrared_async_tx_set_data_isr_callback(ir_jammer_worker_data_isr, worker);

    while(!furi_thread_flags_get() && worker->running) {
        JamMode mode = worker->mode;

        if(mode == JamModeBasic) {
            worker->current_mark = worker->markTiming;
            worker->current_space = worker->markTiming;
            worker->pulses_remaining = 20 * worker->jamDensity; // pairs *2
        } else if(mode == JamModeEnhancedBasic) {
            worker->current_mark = worker->markTiming;
            worker->current_space = worker->spaceTiming;
            worker->pulses_remaining = 20 * worker->jamDensity;
        } else if(mode == JamModeSweep) {
            worker->current_mark = worker->markTiming;
            worker->current_space = worker->markTiming;
            worker->pulses_remaining = 20 * worker->jamDensity;

            // Advance sweep
            worker->markTiming += worker->sweepDir * worker->sweepSpeed;
            if(worker->markTiming > worker->maxTiming || worker->markTiming < worker->minTiming) {
                worker->sweepDir *= -1;
                if(worker->markTiming > worker->maxTiming) worker->markTiming = worker->maxTiming;
                if(worker->markTiming < worker->minTiming) worker->markTiming = worker->minTiming;
            }
        } else if(mode == JamModeRandom) {
            // we can only set one value per burst here without breaking ISR speed heavily,
            // or we generate a random buffer. Let's just use random for the burst
            worker->current_mark = (rand() % 995) + 5;
            worker->current_space = (rand() % 995) + 5;
            worker->pulses_remaining = 30; // some pulses
        } else if(mode == JamModeEmpty) {
            worker->current_mark = 1;
            worker->current_space = 1;
            worker->pulses_remaining = 4 * worker->jamDensity;
        }

        worker->current_level = true;

        furi_hal_light_set(LightGreen, 0xFF);
        furi_hal_infrared_async_tx_start(worker->freq, 0.33);
        furi_hal_infrared_async_tx_wait_termination();
        furi_hal_light_set(LightGreen, 0x00);

        worker->jamCount++;

        // Wait according to Bruce's update intervals based on density roughly
        if(mode == JamModeBasic || mode == JamModeEnhancedBasic) {
            furi_delay_ms(20);
        } else if(mode == JamModeSweep) {
            furi_delay_ms(30);
        } else if(mode == JamModeRandom) {
            furi_delay_ms(100);
        } else if(mode == JamModeEmpty) {
            furi_delay_ms(50);
        } else {
            furi_delay_ms(50);
        }
    }

    furi_hal_infrared_async_tx_set_data_isr_callback(NULL, NULL);
    return 0;
}

IrJammerWorker* ir_jammer_worker_alloc() {
    IrJammerWorker* worker = malloc(sizeof(IrJammerWorker));
    worker->thread = furi_thread_alloc();
    furi_thread_set_name(worker->thread, "IrJammerWorker");
    furi_thread_set_stack_size(worker->thread, 1024);
    furi_thread_set_context(worker->thread, worker);
    furi_thread_set_callback(worker->thread, ir_jammer_worker_thread);

    worker->running = false;
    worker->mode = JamModeBasic;
    worker->freq = 38000;
    worker->markTiming = 12;
    worker->spaceTiming = 12;
    worker->minTiming = 8;
    worker->maxTiming = 70;
    worker->sweepSpeed = 1;
    worker->sweepDir = 1;
    worker->jamDensity = 5;
    worker->jamCount = 0;

    return worker;
}

void ir_jammer_worker_free(IrJammerWorker* worker) {
    if(worker->running) ir_jammer_worker_stop(worker);
    furi_thread_free(worker->thread);
    free(worker);
}

void ir_jammer_worker_start(IrJammerWorker* worker) {
    if(!worker->running) {
        worker->running = true;
        furi_thread_start(worker->thread);
    }
}

void ir_jammer_worker_stop(IrJammerWorker* worker) {
    if(worker->running) {
        worker->running = false;
        furi_thread_flags_set(furi_thread_get_id(worker->thread), 1);
        furi_thread_join(worker->thread);
        furi_hal_light_set(LightGreen, 0x00);
    }
}

void ir_jammer_worker_set_freq(IrJammerWorker* worker, uint32_t freq) {
    worker->freq = freq;
}

void ir_jammer_worker_set_mode(IrJammerWorker* worker, JamMode mode) {
    worker->mode = mode;
}

void ir_jammer_worker_set_basic_timing(IrJammerWorker* worker, uint16_t timing) {
    worker->markTiming = timing;
}

void ir_jammer_worker_set_enhanced_timing(IrJammerWorker* worker, uint16_t mark, uint16_t space) {
    worker->markTiming = mark;
    worker->spaceTiming = space;
}

void ir_jammer_worker_set_sweep_timing(
    IrJammerWorker* worker,
    uint16_t min_timing,
    uint16_t max_timing,
    uint8_t speed) {
    worker->minTiming = min_timing;
    worker->maxTiming = max_timing;
    worker->sweepSpeed = speed;
}

void ir_jammer_worker_set_density(IrJammerWorker* worker, uint8_t density) {
    worker->jamDensity = density;
}

uint32_t ir_jammer_worker_get_jam_count(IrJammerWorker* worker) {
    return worker->jamCount;
}
