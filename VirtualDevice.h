#ifndef VIRTUALDEVICE_H
#define VIRTUALDEVICE_H

#include "Logger.h"
#include "Effect.h"
#include "effects/Cycle.h"
#include "effects/Static.h"

#include <arduino.h>
#include <NeoPixelBus.h>

class VirtualDevice {
    private:
        unsigned long _id;
        String _name;
        size_t _ledCount;

        Effect *_effect = NULL;

        double _posOffsetStart = 0.0;
        double _posOffsetEnd = 1.0;

    public:
        VirtualDevice(size_t ledCount) {
            _id = millis(); // TODO: fix
            _name = "VD#" + String(_id);
            _ledCount = ledCount;

            _effect = new Cycle();
        }

        ~VirtualDevice() {
            if (_effect) {
                delete _effect;
                _effect = NULL;
            }
        }

        VirtualDevice(JsonObject &root, size_t ledCount) {
            _ledCount = ledCount;
            fromConfig(root);
        }

        void update(unsigned long now, RgbColor *colors) {
            // can't do anything without an effect
            if (!_effect)
                return;

            unsigned long duration = _effect->getDuration();
            double timeVal = (double)(now % duration) / duration;

            // double timeVal = (now % 5000) / 5000.0;
            // HsbColor color(timeVal, 1.0, 1.0);

            for (int i = 0; i < _ledCount; i++) {
                double posVal = _posOffsetStart + ((double)i / _ledCount) * (_posOffsetEnd - _posOffsetStart);
                // colors[i] = RgbColor(color);
                colors[i] = _effect->update(timeVal, posVal);
            }
        }

        unsigned long getId() {
            return _id;
        }

        String getName() {
            return _name;
        }

        void setName(String name) {
            _name = name;
        }

        void setLedCount(size_t ledCount) {
            _ledCount = ledCount;
        }

        void setPosOffsetStart(double posOffsetStart) {
            _posOffsetStart = posOffsetStart;
        }

        void setPosOffsetEnd(double posOffsetEnd) {
            _posOffsetEnd = posOffsetEnd;
        }

        void writeConfig(JsonObject &root) {
            root["id"] = _id;
            root["name"] = _name;
            root["posOffsetStart"] = _posOffsetStart;
            root["posOffsetEnd"] = _posOffsetEnd;

            if (_effect)
                root["effectId"] = _effect->getId();

            JsonObject effectDataObj = root.createNestedObject("effectData");
            _effect->writeConfig(effectDataObj);
        }

        void fromConfig(JsonObject &root) {
            _id = root["id"];
            _name = root["name"].as<String>();
            _posOffsetStart = root["posOffsetStart"];
            _posOffsetEnd = root["posOffsetEnd"];

            unsigned long effectId = root["effectId"];
            _effect = EffectManager.createEffect(effectId);

            if (_effect) {
                JsonObject effectDataObj = root["effectData"];
                if (!effectDataObj.isNull()) {
                    _effect->setData(effectDataObj);
                }
            }
        }

};

#endif // VIRTUALDEVICE_H
