#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <ESPAsyncUDP.h>
#include <map>
#include <unordered_set>
#include <functional>

#include "Utils.h"

#define TOPIC_DEVICES 1
#define TOPIC_TIME 2

#define PORT 4567

typedef std::function<void(uint8_t topic, uint8_t *data, size_t length, const IPAddress ip)> OnMessageHandler;

class ESPMessageManager {
    private:
        AsyncUDP _udp;
        std::map<uint8_t, std::unordered_set<HandlerWrapper<OnMessageHandler>>*> _subscriber;
        unsigned long _currSubId = 1;

    public:
        void begin();

        unsigned long subscribe(uint8_t topic, OnMessageHandler callback);
        void unsubscribe(uint8_t topic, unsigned long subId);

        void sendTo(const IPAddress ip, uint8_t topic, uint8_t *data, size_t length);
        void broadcast(uint8_t topic, uint8_t *data, size_t length);
};

extern ESPMessageManager MessageManager;

#endif // MESSAGEMANAGER_H
