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
        size_t _ledCount;

        Effect *_effect = NULL;

        double _posOffsetStart = 0.0;
        double _posOffsetEnd = 1.0;

    public:
        VirtualDevice(size_t ledCount) {
            _id = millis(); // TODO: fix
            _ledCount = ledCount;

            _effect = new Cycle();
        }

        void update(unsigned long now, RgbColor *colors) {
            double timeVal = (now % 5000) / 5000.0;
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

        void setLedCount(size_t ledCount) {
            _ledCount = ledCount;
        }

        void setPosOffsetStart(double posOffsetStart) {
            _posOffsetStart = posOffsetStart;
        }

        void setPosOffsetEnd(double posOffsetEnd) {
            _posOffsetEnd = posOffsetEnd;
        }

};

#endif // VIRTUALDEVICE_H
