#ifndef STATIC_H
#define STATIC_H

#include "../Effect.h"
#include "../PaletteManager.h"
#include "../Utils.h"

#include "../EffectManager.h"

class Static : public Effect {
    private:
        String _paletteName;

    public:
        Static() : Effect(EFFECT_STATIC) {
                // TODO change
                _paletteName = PaletteManager.getPalette(0)->getName();
        }

        void setData(JsonObject &root) {
            unsigned long duration = root["duration"];
            if (duration == 0)
                duration = 5000;

            _duration = duration;


            String paletteName = root["paletteName"];
            if (paletteName.length() > 0) {
                _paletteName = paletteName;
            }
        }

        RgbColor update(double timeVal, double posVal) {
            Palette *palette = PaletteManager.getPalette(_paletteName);

            if (palette) {
                uint32_t color = palette->getColorAtPosition(posVal);
                uint8_t red = colorToRed(color);
                uint8_t green = colorToGreen(color);
                uint8_t blue = colorToBlue(color);

                return RgbColor(red, green, blue);
            }

            return RgbColor(0, 0, 0);
        }

        void writeConfig(JsonObject &root) {
            root["duration"] = _duration;
            root["paletteName"] = _paletteName;
        }
};

#endif // STATIC_H
