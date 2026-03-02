#pragma once

typedef enum {
    IrJammerCustomEventStartscreenUp,
    IrJammerCustomEventStartscreenDown,
    IrJammerCustomEventStartscreenLeft,
    IrJammerCustomEventStartscreenRight,
    IrJammerCustomEventStartscreenOk,
    IrJammerCustomEventStartscreenBack,
    IrJammerCustomEventScene1Up,
    IrJammerCustomEventScene1Down,
    IrJammerCustomEventScene1Left,
    IrJammerCustomEventScene1Right,
    IrJammerCustomEventScene1Ok,
    IrJammerCustomEventScene1Back,
    IrJammerCustomEventScene2Up,
    IrJammerCustomEventScene2Down,
    IrJammerCustomEventScene2Left,
    IrJammerCustomEventScene2Right,
    IrJammerCustomEventScene2Ok,
    IrJammerCustomEventScene2Back,
} IrJammerCustomEvent;

enum IrJammerCustomEventType {
    // Reserve first 100 events for button types and indexes, starting from 0
    IrJammerCustomEventMenuVoid,
    IrJammerCustomEventMenuSelected,
};

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t value;
    } content;
} IrJammerCustomEventMenu;
#pragma pack(pop)

static inline uint32_t ir_jammer_custom_menu_event_pack(uint16_t type, int16_t value) {
    IrJammerCustomEventMenu event = {.content = {.type = type, .value = value}};
    return event.packed_value;
}
static inline void
    ir_jammer_custom_menu_event_unpack(uint32_t packed_value, uint16_t* type, int16_t* value) {
    IrJammerCustomEventMenu event = {.packed_value = packed_value};
    if(type) *type = event.content.type;
    if(value) *value = event.content.value;
}

static inline uint16_t ir_jammer_custom_menu_event_get_type(uint32_t packed_value) {
    uint16_t type;
    ir_jammer_custom_menu_event_unpack(packed_value, &type, NULL);
    return type;
}

static inline int16_t ir_jammer_custom_menu_event_get_value(uint32_t packed_value) {
    int16_t value;
    ir_jammer_custom_menu_event_unpack(packed_value, NULL, &value);
    return value;
}