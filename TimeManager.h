#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <arduino.h>

#define TIME_SYNC_INTERVAL 60000

class ESPTimeManager {
    private:
        unsigned long _syncedMillisOffset = 0;
        bool _syncResponseEnabled = true;

        unsigned long _lastTimeSyncRequestMillis = 0;
        bool _firstSyncRequest = true;

        bool _isFirst = true;
        double _avgDiff = 0;

    public:
        void begin();
        void update();
        void timeSyncRequest();
        void setTimeSyncResponseEnabled(bool enabled);
        unsigned long syncedMillis();

};

extern ESPTimeManager TimeManager;

#endif // TIMEMANAGER_H
