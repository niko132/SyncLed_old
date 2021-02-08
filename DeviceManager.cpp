#include "DeviceManager.h"

#include "Logger.h"
#include "MessageManager.h"
#include "Utils.h"

void ESPDeviceManager::begin() {
    MessageManager.subscribe(TOPIC_DEVICES, [this](uint8_t topic, uint8_t *data, size_t length, const IPAddress ip) {
        Reader reader(data, length);

        uint8_t type;
        reader.read(&type);

        if (type == 1) { // Heartbeat received
            std::map<IPAddress, unsigned long>::iterator it = _activeDevices.find(ip);

            if (it == _activeDevices.end()) {
                Logger.print(ip);
                Logger.println(" is online");


                std::unordered_set<HandlerWrapper<OnDeviceChangeHandler>>::iterator it = _handler.begin();

                while(it != _handler.end()) {
                    std::unordered_set<HandlerWrapper<OnDeviceChangeHandler>>::iterator current = it++;

                    OnDeviceChangeHandler callback = (*current).fn;
                    callback(ip, true);
                }
            }

            _activeDevices[ip] = millis();
        } else if (type == 2) { // Heartbeat request
            sendHeartbeat();
        } else {
            Logger.println("UNKNOWN COMMAND");
        }
    });

    requestHeartbeat();
}

void ESPDeviceManager::update() {
    unsigned long now = millis();

    if (now - _lastHeartbeatMillis > HEARTBEAT_INTERVALL) {
        sendHeartbeat();
        _lastHeartbeatMillis = now;
    }

    std::map<IPAddress, unsigned long>::iterator it = _activeDevices.begin();

    while(it != _activeDevices.end()) {
        std::map<IPAddress, unsigned long>::iterator current = it++;

        if (now - current->second > 3.5 * HEARTBEAT_INTERVALL) {
            IPAddress ip = current->first;
            Logger.print(ip);
            Logger.println(" is offline");

            // suspect the device to be offline
            _activeDevices.erase(current);


            std::unordered_set<HandlerWrapper<OnDeviceChangeHandler>>::iterator it = _handler.begin();

            while(it != _handler.end()) {
                std::unordered_set<HandlerWrapper<OnDeviceChangeHandler>>::iterator current = it++;

                OnDeviceChangeHandler callback = (*current).fn;
                callback(ip, false);
            }
        }
    }
}


unsigned long ESPDeviceManager::addDeviceChangeHandler(OnDeviceChangeHandler handler) {
    unsigned long id = _currentHandlerId++;
    _handler.insert(HandlerWrapper<OnDeviceChangeHandler>(id, handler));
    return id;
}

void ESPDeviceManager::removeDeviceChangeHandler(unsigned long handlerId) {
    HandlerWrapper<OnDeviceChangeHandler> tmp(handlerId);
    _handler.erase(tmp);
}


void ESPDeviceManager::sendHeartbeat() {
    uint8_t buffer = 1;
    MessageManager.broadcast(TOPIC_DEVICES, &buffer, sizeof(buffer));
}

void ESPDeviceManager::requestHeartbeat() {
    uint8_t buffer = 2;
    MessageManager.broadcast(TOPIC_DEVICES, &buffer, sizeof(buffer));
}


size_t ESPDeviceManager::getActiveDeviceCount() {
    return _activeDevices.size();
}

std::set<IPAddress> ESPDeviceManager::getActiveDevices() {
    std::set<IPAddress> devices;

    for (std::map<IPAddress, unsigned long>::iterator it = _activeDevices.begin(); it != _activeDevices.end(); it++) {
        devices.insert(it->first);
    }

    return devices;
}

ESPDeviceManager DeviceManager;
