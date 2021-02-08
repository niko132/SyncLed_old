#ifndef LOGGER_H
#define LOGGER_H

#include <Print.h>
#include <WiFiUdp.h>
#include <Syslog.h>

class ESPLogger : public Print {
    private:
        WiFiUDP _udpClient;
        Syslog _syslog;

        String _currLine;
        uint16_t _maxPrio;

        void updatePrio(uint16_t newPrio);
        void sendCurrentLine();

    public:
        ESPLogger();

        size_t write(uint8_t);
        using Print::write;

        size_t print(const __FlashStringHelper *, uint16_t = LOG_INFO);
        size_t print(const String &, uint16_t = LOG_INFO);
        size_t print(const char[], uint16_t = LOG_INFO);
        size_t print(char, uint16_t = LOG_INFO);
        size_t print(unsigned char, int = DEC, uint16_t = LOG_INFO);
        size_t print(int, int = DEC, uint16_t = LOG_INFO);
        size_t print(unsigned int, int = DEC, uint16_t = LOG_INFO);
        size_t print(long, int = DEC, uint16_t = LOG_INFO);
        size_t print(unsigned long, int = DEC, uint16_t = LOG_INFO);
        size_t print(double, int = 2, uint16_t = LOG_INFO);
        size_t print(const Printable&, uint16_t = LOG_INFO);

        size_t println(const __FlashStringHelper *, uint16_t = LOG_INFO);
        size_t println(const String &s, uint16_t = LOG_INFO);
        size_t println(const char[], uint16_t = LOG_INFO);
        size_t println(char, uint16_t = LOG_INFO);
        size_t println(unsigned char, int = DEC, uint16_t = LOG_INFO);
        size_t println(int, int = DEC, uint16_t = LOG_INFO);
        size_t println(unsigned int, int = DEC, uint16_t = LOG_INFO);
        size_t println(long, int = DEC, uint16_t = LOG_INFO);
        size_t println(unsigned long, int = DEC, uint16_t = LOG_INFO);
        size_t println(double, int = 2, uint16_t = LOG_INFO);
        size_t println(const Printable&, uint16_t = LOG_INFO);
        size_t println(void);
};

extern ESPLogger Logger;

#endif
