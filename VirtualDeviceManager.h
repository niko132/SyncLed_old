#ifndef VIRTUALDEVICEMANAGER_H
#define VIRTUALDEVICEMANAGER_H

#include "VirtualDevice.h"

#include <NeoPixelBus.h>

#include <map>

class VirtualDeviceWrapper {
    private:
        VirtualDevice *_device;
        size_t _startIndex;
        size_t _endIndex;

    public:

        VirtualDeviceWrapper(size_t startIndex, size_t endIndex) {
            _startIndex = startIndex;
            _endIndex = endIndex;
            _device = new VirtualDevice(getLedCount());
        }

        ~VirtualDeviceWrapper() {
            if (_device) {
                delete _device;
                _device = NULL;
            }
        }

        VirtualDevice* getVirtualDevice() {
            return _device;
        }

        size_t getStartIndex() {
            return _startIndex;
        }

        size_t getEndIndex() {
            return _endIndex;
        }

        void setStartIndex(size_t startIndex) {
            _startIndex = startIndex;
            _device->setLedCount(getLedCount());
        }

        void setEndIndex(size_t endIndex) {
            _endIndex = endIndex;
            _device->setLedCount(getLedCount());
        }

        size_t getLedCount() {
            return _endIndex - _startIndex;
        }
};

class ESPVirtualDeviceManager {
    private:
        size_t _ledCount;
        RgbColor *_colors;
        NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *_strip;
        std::map<unsigned long, VirtualDeviceWrapper*> _virtualDevices;

        unsigned long _lastUpdate = 0;

        void ensure(size_t startIndex, size_t endIndex, VirtualDeviceWrapper *except = NULL);
        void logConfig();

    public:
        void begin(size_t ledCount);
        void update();
        unsigned long addVirtualDevice(size_t startIndex, size_t endIndex);
        void deleteVirtualDevice(unsigned long id);
        void setStartIndex(unsigned long id, size_t startIndex);
        void setEndIndex(unsigned long id, size_t endIndex);
        size_t getVirtualDeviceCount();
        VirtualDevice* getVirtualDevice(unsigned long id);
};

extern ESPVirtualDeviceManager VirtualDeviceManager;

#endif // VIRTUALDEVICEMANAGER_H
