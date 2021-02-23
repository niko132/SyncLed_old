#ifndef EFFECTMANAGER_H
#define EFFECTMANAGER_H

#include "Effect.h"

#define EFFECT_CYCLE 1
#define EFFECT_STATIC 2

class ESPEffectManager {
    public:
        Effect* createEffect(unsigned long id);
};

extern ESPEffectManager EffectManager;

#endif // EFFECTMANAGER_H
