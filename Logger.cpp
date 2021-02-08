#include "Logger.h"

#include <ESP8266WiFi.h>


ESPLogger::ESPLogger() : _syslog(_udpClient, "255.255.255.255", 514) {
    _currLine = "";
    _maxPrio = LOG_DEBUG;
};

void ESPLogger::updatePrio(uint16_t newPrio) {
    // prios are reversed order
    // lowest prio is highest importance rating
    if (newPrio < _maxPrio) {
        _maxPrio = newPrio;
    }
}


void ESPLogger::sendCurrentLine() {
    _syslog.log(_maxPrio, _currLine);
    Serial.print(_currLine);

    _currLine = "";
    _maxPrio = LOG_DEBUG;
}



size_t ESPLogger::write(uint8_t c) {
    _currLine += (char)c;
    return 1;
}


size_t ESPLogger::print(const __FlashStringHelper *fsh, uint16_t prio) {
    updatePrio(prio);
    return Print::print(fsh);
}
size_t ESPLogger::print(const String &s, uint16_t prio) {
    updatePrio(prio);
    return Print::print(s);
}
size_t ESPLogger::print(const char ca[], uint16_t prio) {
    updatePrio(prio);
    return Print::print(ca);
}
size_t ESPLogger::print(char c, uint16_t prio) {
    updatePrio(prio);
    return Print::print(c);
}
size_t ESPLogger::print(unsigned char c, int f, uint16_t prio) {
    updatePrio(prio);
    return Print::print(c, f);
}
size_t ESPLogger::print(int n, int f, uint16_t prio) {
    updatePrio(prio);
    return Print::print(n, f);
}
size_t ESPLogger::print(unsigned int n, int f, uint16_t prio) {
    updatePrio(prio);
    return Print::print(n, f);
}
size_t ESPLogger::print(long n, int f, uint16_t prio) {
    updatePrio(prio);
    return Print::print(n, f);
}
size_t ESPLogger::print(unsigned long n, int f, uint16_t prio) {
    updatePrio(prio);
    return Print::print(n, f);
}
size_t ESPLogger::print(double n, int f, uint16_t prio) {
    updatePrio(prio);
    return Print::print(n, f);
}
size_t ESPLogger::print(const Printable &p, uint16_t prio) {
    updatePrio(prio);
    return Print::print(p);
}

size_t ESPLogger::println(const __FlashStringHelper *fsh, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(fsh);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(const String &s, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(s);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(const char ca[], uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(ca);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(char c, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(c);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(unsigned char c, int f, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(c, f);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(int n, int f, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(n, f);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(unsigned int n, int f, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(n, f);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(long n, int f, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(n, f);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(unsigned long n, int f, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(n, f);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(double n, int f, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(n, f);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println(const Printable &p, uint16_t prio) {
    updatePrio(prio);
    size_t ret = Print::println(p);
    sendCurrentLine();
    return ret;
}
size_t ESPLogger::println() {
    size_t ret = Print::println();
    sendCurrentLine();
    return ret;
}

ESPLogger Logger;
