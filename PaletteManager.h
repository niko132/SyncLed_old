#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include "Palette.h"

#include <map>

class ESPPaletteManager {
    private:
        std::map<String, Palette*> _palettes;

    public:
        Palette* createPalette(String name);
        void renamePalette(String oldName, String newName);
        void deletePalette(String name);
        Palette* getPalette(String name);
};

extern ESPPaletteManager PaletteManager;

#endif // PALETTEMANAGER_H
