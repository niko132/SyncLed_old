#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include "Palette.h"

#include <ArduinoJson.h>
#include <map>

class ESPPaletteManager {
    private:
        std::map<String, Palette*> _palettes;

    public:
        Palette* createPalette(String name);
        void renamePalette(String oldName, String newName);
        void deletePalette(String name);
        Palette* getPalette(String name);
        Palette* getPalette(size_t index);

        void writeConfig(JsonObject &root);

        bool fromConfig(JsonObject &root);
};

extern ESPPaletteManager PaletteManager;

#endif // PALETTEMANAGER_H
