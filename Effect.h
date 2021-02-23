#ifndef EFFECT_H
#define EFFECT_H

#include <NeoPixelBus.h>
#include <ArduinoJson.h>

class Effect {
    private:
        unsigned long _id;

    protected:
        unsigned long _duration;

    public:
        Effect(unsigned long id) {
            _id = id;
            _duration = 5000; // default duration is 5secs
        }

        unsigned long getId() {
            return _id;
        }

        virtual unsigned long getDuration() {
            return _duration;
        }

        virtual void setData(JsonObject &root) = 0;
        virtual RgbColor update(double timeVal, double posVal) = 0;

        virtual void writeConfig(JsonObject &root) = 0;
};

#endif // EFFECT_H
