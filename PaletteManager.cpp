#include "PaletteManager.h"

Palette* ESPPaletteManager::createPalette(String name) {
    Palette *palette = new Palette(name);
    _palettes[name] = palette;
    return palette;
}

void ESPPaletteManager::renamePalette(String oldName, String newName) {
    Palette *palette = _palettes[oldName];

    if (palette) {
        palette->setName(newName);
        _palettes.erase(oldName);
        _palettes[newName] = palette;
    }
}

void ESPPaletteManager::deletePalette(String name) {
    Palette *palette = _palettes[name];

    if (palette) {
        delete palette;
        _palettes.erase(name);
    }
}

Palette* ESPPaletteManager::getPalette(String name) {
    return _palettes[name];
}


ESPPaletteManager PaletteManager;
