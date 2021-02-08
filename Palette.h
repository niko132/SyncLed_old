#ifndef PALETTE_H
#define PALETTE_H

#include <arduino.h>
#include <map>

class Palette {
    private:
        String _name;
        std::map<double, uint32_t> _colors;

    public:
        Palette(String name);

        String getName();
        void setName(String name);

        void addColorKey(double pos, uint32_t color);
        uint32_t getColorAtPosition(double pos);
};

#endif // PALETTE_H
