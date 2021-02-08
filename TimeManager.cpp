#include "TimeManager.h"

#include "Logger.h"
#include "MessageManager.h"
#include "DeviceManager.h"
#include "Utils.h"

#include <Arduino.h>

void ESPTimeManager::begin() {
    setTimeSyncResponseEnabled(false);

    MessageManager.subscribe(TOPIC_TIME, [this](uint8_t topic, uint8_t *data, size_t length, const IPAddress ip) {
        unsigned long messageReceiveMillis = syncedMillis();

        Reader reader(data, length);

        uint8_t type;
        reader.read(&type);


        if (type == 1) { // timeSyncRequest
            Logger.println("Time Sync Request");

            if (!_syncResponseEnabled) {
                // ignore a time sync request
                return;
            }

            unsigned long remoteMillis;
            reader.read(&remoteMillis);


            Writer writer;

            uint8_t type = 2;
            writer.write(type);
            writer.write(remoteMillis);
            writer.write(messageReceiveMillis);
            unsigned long currentMillis = syncedMillis();
            writer.write(currentMillis);

            MessageManager.sendTo(ip, TOPIC_TIME, writer.getData(), writer.getLength());
        } else if (type == 2) { // timeSyncResponse
            Logger.println("Time Sync Response");

            unsigned long localMillis;
            unsigned long remoteStartMillis;
            unsigned long remoteEndMillis;

            reader.read(&localMillis);
            reader.read(&remoteStartMillis);
            reader.read(&remoteEndMillis);


            unsigned long t0 = localMillis;
            unsigned long t1 = remoteStartMillis;
            unsigned long t2 = remoteEndMillis;
            unsigned long t3 = messageReceiveMillis;

            long offset = (long)(((long)((t1 - t0) + (t2 - t3))) / 2.0);
            unsigned long rtd = (t3 - t0) - (t2 - t1);

            unsigned long remoteMillisCorrected = (unsigned long)(remoteEndMillis + rtd / 2.0);

            long diff = remoteMillisCorrected - messageReceiveMillis;


            if (_isFirst) {
                _isFirst = false;
            } else {
                _avgDiff = 0.7 * _avgDiff + 0.3 * diff;
            }


            Logger.println("RTD: " + String(rtd));
            Logger.println("Remote Time: " + String(remoteMillisCorrected) + " Local Time: " + String(messageReceiveMillis) + " Diff: " + String((long)(remoteMillisCorrected - messageReceiveMillis)));
            Logger.println("Avg Diff: " + String(_avgDiff));
            Logger.println();

            _syncedMillisOffset += offset;


            //this device now has the correct time -> can be used to sync other devices
            setTimeSyncResponseEnabled(true);
        } else {
            Logger.println("UNKNOWN COMMAND");
        }
    });
}

void ESPTimeManager::update() {
    unsigned long now = millis();
    size_t deviceCount = DeviceManager.getActiveDeviceCount();


    if (!_syncResponseEnabled && now > TIME_SYNC_INTERVAL / 2) {
        setTimeSyncResponseEnabled(true);
        Logger.println("Time sync response is enabled");
    }


    if (deviceCount && (_firstSyncRequest || now - _lastTimeSyncRequestMillis > TIME_SYNC_INTERVAL)) {
        timeSyncRequest();
        _firstSyncRequest = false;
        _lastTimeSyncRequestMillis = now;
    }
}

void ESPTimeManager::timeSyncRequest() {
    Logger.println("Sending Time Sync Request...");

    size_t deviceCount = DeviceManager.getActiveDeviceCount();

    if (deviceCount) {
        std::set<IPAddress> devices = DeviceManager.getActiveDevices();
        std::set<IPAddress>::iterator it = devices.begin();
        int r = random(deviceCount);

        while(r--) {
            it++;
        }


        IPAddress device = *it;

        Writer writer;

        uint8_t type = 1;
        writer.write(type);
        unsigned long currentMillis = syncedMillis();
        writer.write(currentMillis);

        MessageManager.sendTo(device, TOPIC_TIME, writer.getData(), writer.getLength());
    }
}

void ESPTimeManager::setTimeSyncResponseEnabled(bool enabled) {
    _syncResponseEnabled = enabled;
}

unsigned long ESPTimeManager::syncedMillis() {
    return millis() + _syncedMillisOffset;
}

ESPTimeManager TimeManager;
