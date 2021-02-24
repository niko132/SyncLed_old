#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <Ethernet.h>
#include <ArduinoJson.h>

#include <map>
#include <set>
#include <unordered_set>

#include "Utils.h"

#define HEARTBEAT_INTERVALL 1000

typedef std::function<void(IPAddress ip, bool online)> OnDeviceChangeHandler;

namespace std {
    template<>
    struct less<IPAddress> {
        public:
            bool operator()(const IPAddress &lhs, const IPAddress &rhs) const {
                return std::less<String>()(lhs.toString(), rhs.toString());
            }
    };
}

class ESPDeviceManager {
    private:
        unsigned long _lastHeartbeatMillis;
        std::map<IPAddress, unsigned long> _activeDevices;

        unsigned long _currentHandlerId = 0;
        std::unordered_set<HandlerWrapper<OnDeviceChangeHandler>> _handler;

        String _config;

        void sendHeartbeat();
        void requestHeartbeat();

    public:
        void begin();
        void update();

        unsigned long addDeviceChangeHandler(OnDeviceChangeHandler handler);
        void removeDeviceChangeHandler(unsigned long handlerId);

        size_t getActiveDeviceCount();
        std::set<IPAddress> getActiveDevices();

        void writeConfig(JsonObject &root);
        bool fromConfig(JsonObject &root);
};

extern ESPDeviceManager DeviceManager;

#endif // DEVICEMANAGER_H
