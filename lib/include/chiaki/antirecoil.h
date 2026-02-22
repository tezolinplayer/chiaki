#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <chiaki/controller.h>
#include <chiaki/ctrl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool enabled;
    float vertical_strength;
    int fire_button;
} AntiRecoilConfig;

void chiaki_antirecoil_init(void);
void chiaki_antirecoil_process(ChiakiControllerState *state);

#ifdef __cplusplus
}
#endif
