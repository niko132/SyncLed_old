#include "VirtualDeviceManager.h"

#include "Logger.h"
#include "TimeManager.h"

void ESPVirtualDeviceManager::begin(size_t ledCount) {
    _ledCount = ledCount;
    _colors = new RgbColor[ledCount];
    _strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(ledCount);

    _strip->Begin();
    _strip->Show();
}

void ESPVirtualDeviceManager::update() {
    unsigned long mills = millis();

    if (mills - _lastUpdate > 1000.0 / 60.0) {
        unsigned long now = TimeManager.syncedMillis();

        for (std::map<unsigned long, VirtualDeviceWrapper*>::iterator it = _virtualDevices.begin(); it != _virtualDevices.end(); it++) {
            VirtualDeviceWrapper *wrapper = it->second;

            if (wrapper) {
                wrapper->getVirtualDevice()->update(now, &_colors[wrapper->getStartIndex()]);
            }
        }

        for (int i = 0; i < _ledCount; i++) {
            _strip->SetPixelColor(i, _colors[i]);
        }

        _strip->Show();



        _lastUpdate = mills;
    }
}

unsigned long ESPVirtualDeviceManager::addVirtualDevice(size_t startIndex, size_t endIndex) {
    if (startIndex < 0) {
        startIndex = 0;
    }

    if (endIndex > _ledCount) {
        endIndex = _ledCount;
    }

    ensure(startIndex, endIndex);

    VirtualDeviceWrapper *wrapper = new VirtualDeviceWrapper(startIndex, endIndex);
    unsigned long id = wrapper->getVirtualDevice()->getId();
    _virtualDevices[id] = wrapper;

    logConfig();

    return id;
}

void ESPVirtualDeviceManager::deleteVirtualDevice(unsigned long id) {
    VirtualDeviceWrapper *wrapper = _virtualDevices[id];

    if (wrapper) {
        delete wrapper;
    }

    _virtualDevices.erase(id);

    logConfig();
}

void ESPVirtualDeviceManager::setStartIndex(unsigned long id, size_t startIndex) {
    if (startIndex < 0) {
        startIndex = 0;
    }

    VirtualDeviceWrapper *wrapper = _virtualDevices[id];

    if (wrapper) {
        size_t endIndex = wrapper->getEndIndex();
        ensure(startIndex, endIndex, wrapper);
        wrapper->setStartIndex(startIndex);
    }

    logConfig();
}

void ESPVirtualDeviceManager::setEndIndex(unsigned long id, size_t endIndex) {
    if (endIndex > _ledCount) {
        endIndex = _ledCount;
    }

    VirtualDeviceWrapper *wrapper = _virtualDevices[id];

    if (wrapper) {
        size_t startIndex = wrapper->getStartIndex();
        ensure(startIndex, endIndex, wrapper);
        wrapper->setEndIndex(endIndex);
    }

    logConfig();
}

size_t ESPVirtualDeviceManager::getVirtualDeviceCount() {
    return _virtualDevices.size();
}

VirtualDevice* ESPVirtualDeviceManager::getVirtualDevice(unsigned long id) {
    VirtualDeviceWrapper *wrapper = _virtualDevices[id];

    if (wrapper) {
        return wrapper->getVirtualDevice();
    }

    return NULL;
}


void ESPVirtualDeviceManager::ensure(size_t startIndex, size_t endIndex, VirtualDeviceWrapper *except) {
    std::map<unsigned long, VirtualDeviceWrapper*>::iterator it = _virtualDevices.begin();

    while(it != _virtualDevices.end()) {
        std::map<unsigned long, VirtualDeviceWrapper*>::iterator current = it++;
        VirtualDeviceWrapper *wrapper = current->second;

        if (wrapper == except) {
            continue;
        }

        if (startIndex <= wrapper->getStartIndex() && endIndex >= wrapper->getEndIndex()) {
            delete wrapper;
            _virtualDevices.erase(current);
        } else if (startIndex > wrapper->getStartIndex() && endIndex < wrapper->getEndIndex()) {
            delete wrapper;
            _virtualDevices.erase(current);
        } else if (startIndex < wrapper->getStartIndex() && endIndex > wrapper->getStartIndex() && endIndex < wrapper->getEndIndex()) {
            wrapper->setStartIndex(endIndex);
        } else if(startIndex > wrapper->getStartIndex() && startIndex < wrapper->getEndIndex() && endIndex > wrapper->getEndIndex()) {
            wrapper->setEndIndex(startIndex);
        }
    }
}


void ESPVirtualDeviceManager::logConfig() {
    Logger.print(_virtualDevices.size());
    Logger.print(": ");

    for (std::map<unsigned long, VirtualDeviceWrapper*>::iterator it = _virtualDevices.begin(); it != _virtualDevices.end(); it++) {
        unsigned long id = it->first;
        VirtualDeviceWrapper *wrapper = it->second;

        if (wrapper) {
            Logger.print(id);
            Logger.print(" (");
            Logger.print(wrapper->getStartIndex());
            Logger.print(", ");
            Logger.print(wrapper->getEndIndex());
            Logger.print(") ");
        } else {
            Logger.print("NULL ");
        }
    }

    Logger.println();
}


ESPVirtualDeviceManager VirtualDeviceManager;
