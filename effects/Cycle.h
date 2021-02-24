#ifndef CYCLE_H
#define CYCLE_H

#include "../Effect.h"
#include "../PaletteManager.h"
#include "../Utils.h"

#include "../EffectManager.h"

class Cycle : public Effect {
    private:
        String _paletteName;

    public:
        Cycle() : Effect(EFFECT_CYCLE) {
            // TODO change
            _paletteName = PaletteManager.getPalette(0)->getName();
        }

        void setData(JsonObject &root) {
            Effect::setData(root);

            String paletteName = root["paletteName"];
            if (paletteName.length() > 0) {
                _paletteName = paletteName;
            }
        }

        RgbColor update(double timeVal, double posVal) {
            double val = posVal - timeVal;
            if (val < 0.0) {
                val += 1.0;
            }

            Palette *palette = PaletteManager.getPalette(_paletteName);

            if (palette) {
                uint32_t color = palette->getColorAtPosition(val);
                uint8_t red = colorToRed(color);
                uint8_t green = colorToGreen(color);
                uint8_t blue = colorToBlue(color);

                return RgbColor(red, green, blue);
            }

            return RgbColor(0, 0, 0);
        }

        void writeConfig(JsonObject &root) {
            Effect::writeConfig(root);

            root["paletteName"] = _paletteName;
        }
};

#endif // CYCLE_H
