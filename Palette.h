#ifndef PALETTE_H
#define PALETTE_H

#include <arduino.h>
#include <ArduinoJson.h>
#include <map>

class Palette {
    private:
        String _name;
        std::map<double, uint32_t> _colors;

    public:
        Palette(String name);
        Palette(JsonObject &root);

        String getName();
        void setName(String name);

        void addColorKey(double pos, uint32_t color);
        uint32_t getColorAtPosition(double pos);

        void writeConfig(JsonObject &root);
};

#endif // PALETTE_H
