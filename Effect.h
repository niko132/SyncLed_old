#ifndef EFFECT_H
#define EFFECT_H

#include <NeoPixelBus.h>
#include <ArduinoJson.h>

#define DEFAULT_DURATION 5000 // 5 secs

class Effect {
    private:
        unsigned long _id;

    protected:
        unsigned long _duration;

    public:
        Effect(unsigned long id) {
            _id = id;
            _duration = DEFAULT_DURATION;
        }

        unsigned long getId() {
            return _id;
        }

        virtual unsigned long getDuration() {
            return _duration;
        }

        virtual RgbColor update(double timeVal, double posVal) = 0;

        virtual void setData(JsonObject &root) {
            unsigned long duration = root["duration"];
            if (duration == 0)
                duration = DEFAULT_DURATION;

            _duration = duration;
        }

        virtual void writeConfig(JsonObject &root) {
                root["duration"] = _duration;
        }
};

#endif // EFFECT_H
