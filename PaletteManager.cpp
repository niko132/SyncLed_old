#include "PaletteManager.h"

#include "Logger.h"

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

Palette* ESPPaletteManager::getPalette(size_t index) {
    for (std::map<String, Palette*>::iterator it = _palettes.begin(); it != _palettes.end(); it++, index--) {
        if (index == 0) {
            return it->second;
        }
    }

    return NULL;
}

void ESPPaletteManager::writeConfig(JsonObject &root) {
    JsonArray palettesArr = root.createNestedArray("palettes");

    for (std::map<String, Palette*>::iterator it = _palettes.begin(); it != _palettes.end(); it++) {
        Palette *palette = it->second;

        JsonObject paletteObj = palettesArr.createNestedObject();
        palette->writeConfig(paletteObj);
    }
}

bool ESPPaletteManager::fromConfig(JsonObject &root) {
    JsonArray palettesArr = root["palettes"].as<JsonArray>();
    if (palettesArr.isNull()) {
        Logger.println("PalettesArray is null");
        return false;
    }

    std::map<String, Palette*>::iterator it = _palettes.begin();
    while (it != _palettes.end()) {
        std::map<String, Palette*>::iterator current = it++;

        Palette *palette = current->second;
        if (palette) {
            delete palette;
        }

        _palettes.erase(current);
    }

    for (JsonVariant paletteVar : palettesArr) {
        JsonObject paletteObj = paletteVar.as<JsonObject>();

        if (paletteObj.isNull()) {
            continue;
        }

        Palette *palette = new Palette(paletteObj);
        _palettes[palette->getName()] = palette;
    }

    return true;
}


ESPPaletteManager PaletteManager;
