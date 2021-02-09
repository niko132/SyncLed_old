#include "Palette.h"

#include "Utils.h"

Palette::Palette(String name) {
    _name = name;
}

Palette::Palette(JsonObject &root) {
    _name = root["name"].as<String>();

    JsonArray keysArr = root["keys"].as<JsonArray>();
    if (!keysArr.isNull()) {
        for (JsonVariant keyVar : keysArr) {
            JsonObject keyObj = keyVar.as<JsonObject>();
            if (keyObj.isNull()) {
                continue;
            }

            JsonVariant posVar = keyObj["pos"];
            JsonVariant colorVar = keyObj["color"];

            if (!posVar.isNull() && !colorVar.isNull()) {
                double pos = posVar.as<double>();
                uint32_t color = colorVar.as<uint32_t>();

                _colors[pos] = color;
            }
        }
    }
}

String Palette::getName() {
    return _name;
}

void Palette::setName(String name) {
    _name = name;
}

void Palette::addColorKey(double pos, uint32_t color) {
    _colors[pos] = color;
}

uint32_t Palette::getColorAtPosition(double pos) {
    double firstPos;
    double secondPos;

    uint32_t firstColor;
    uint32_t secondColor;


    if (pos < _colors.begin()->first) {
        std::map<double, uint32_t>::reverse_iterator first = _colors.rbegin();
        std::map<double, uint32_t>::iterator second = _colors.begin();

        firstPos = first->first - 1.0;
        secondPos = second->first;

        firstColor = first->second;
        secondColor = second->second;
    } else if (pos >= _colors.rbegin()->first) {
        std::map<double, uint32_t>::reverse_iterator first = _colors.rbegin();
        std::map<double, uint32_t>::iterator second = _colors.begin();

        firstPos = first->first;
        secondPos = second->first + 1.0;

        firstColor = first->second;
        secondColor = second->second;
    } else {
        std::map<double, uint32_t>::iterator it = _colors.begin();

        while (it != _colors.end()) {
            std::map<double, uint32_t>::iterator current = it++;

            if (current->first <= pos) {
                if (it->first > pos) {
                    firstPos = current->first;
                    secondPos = it->first;

                    firstColor = current->second;
                    secondColor = it->second;
                }
            }
        }
    }

    double frac = (pos - firstPos) / (secondPos - firstPos);

    // now blend between firstColor and secondColor with a value of frac
    uint8_t firstRed = colorToRed(firstColor);
    uint8_t firstGreen = colorToGreen(firstColor);
    uint8_t firstBlue = colorToBlue(firstColor);

    uint8_t secondRed = colorToRed(secondColor);
    uint8_t secondGreen = colorToGreen(secondColor);
    uint8_t secondBlue = colorToBlue(secondColor);

    uint8_t blendedRed = firstRed + (secondRed - firstRed) * frac;
    uint8_t blendedGreen = firstGreen + (secondGreen - firstGreen) * frac;
    uint8_t blendedBlue = firstBlue + (secondBlue - firstBlue) * frac;

    return rgbToColor(blendedRed, blendedGreen, blendedBlue);
}

void Palette::writeConfig(JsonObject &root) {
    root["name"] = _name;
    JsonArray keysArr = root.createNestedArray("keys");

    for (std::map<double, uint32_t>::iterator it = _colors.begin(); it != _colors.end(); it++) {
        JsonObject keyObj = keysArr.createNestedObject();
        keyObj["pos"] = it->first;
        keyObj["color"] = it->second;
    }
}
