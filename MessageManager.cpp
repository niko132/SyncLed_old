#include "MessageManager.h"

#include "Logger.h"

void ESPMessageManager::begin() {
    if (_udp.listen(PORT)) {
        Logger.println("UDP listening...");

        _udp.onPacket([this](AsyncUDPPacket packet) {
            // Logger.println("Packet received");

            uint8_t *data = packet.data();
            size_t size = packet.length();

            size_t pos = 0;

            uint8_t topic;
            memcpy(&topic, data, sizeof(topic));
            pos += sizeof(topic);

            // Logger.print("Topic: ");
            // Logger.println((int)topic);

            size_t numSubs;
            std::unordered_set<HandlerWrapper<OnMessageHandler>> *topicSet = _subscriber[topic];
            if (topicSet) {
                numSubs = topicSet->size();
            } else {
                numSubs = 0;
            }

            // Logger.print("Subs: ");
            // Logger.println((int)numSubs);

            if (topicSet) {
                std::unordered_set<HandlerWrapper<OnMessageHandler>>::iterator it = topicSet->begin();

                while(it != topicSet->end()) {
                    std::unordered_set<HandlerWrapper<OnMessageHandler>>::iterator current = it++;

                    OnMessageHandler callback = (*current).fn;
                    callback(topic, data + pos, size - pos, packet.remoteIP());
                }
            }
        });
    } else {
        Logger.println("Failed to listen UDP");
    }
}

unsigned long ESPMessageManager::subscribe(uint8_t topic, OnMessageHandler callback) {
    std::unordered_set<HandlerWrapper<OnMessageHandler>> *topicSet = _subscriber[topic];
    if (!topicSet) {
        topicSet = new std::unordered_set<HandlerWrapper<OnMessageHandler>>();
        _subscriber[topic] = topicSet;
    }

    unsigned long id = _currSubId++;

    topicSet->insert(HandlerWrapper<OnMessageHandler>(id, callback));
    return id;
}

void ESPMessageManager::unsubscribe(uint8_t topic, unsigned long subId) {
    std::unordered_set<HandlerWrapper<OnMessageHandler>> *topicSet = _subscriber[topic];

    if (!topicSet) {
        return;
    }

    // create a temp object with the specified id
    // the set will just compare the ids and delete the element with the specified id
    HandlerWrapper<OnMessageHandler> tmp(subId);

    topicSet->erase(tmp);

    if (topicSet->empty()) {
        delete topicSet;
        _subscriber.erase(topic);
    }
}

void ESPMessageManager::sendTo(const IPAddress ip, uint8_t topic, uint8_t *data, size_t length) {
    size_t size = length + sizeof(topic);
    uint8_t *buffer = new uint8_t[size];

    buffer[0] = topic;
    memcpy(&buffer[1], data, length);

    _udp.writeTo(buffer, size, ip, PORT);

    delete[] buffer;
}

void ESPMessageManager::broadcast(uint8_t topic, uint8_t *data, size_t length) {
    size_t size = length + sizeof(topic);
    uint8_t *buffer = new uint8_t[size];

    buffer[0] = topic;
    memcpy(&buffer[1], data, length);

    _udp.broadcastTo(buffer, size, PORT);

    delete[] buffer;
}

ESPMessageManager MessageManager;
