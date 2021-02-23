#include "EffectManager.h"

#include "effects/Cycle.h"
#include "effects/Static.h"

Effect* ESPEffectManager::createEffect(unsigned long id) {
    switch (id) {
        case EFFECT_CYCLE:
            return new Cycle();
        case EFFECT_STATIC:
            return new Static();
        default:
            return NULL;
    }
}

ESPEffectManager EffectManager;
