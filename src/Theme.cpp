#include "Theme.h"

const AccentPalette& Theme::accents()
{
    static AccentPalette palette{
        QColor("#5f8cec"),
        QColor("#a784e9"),
        QColor("#f56982"),
        QColor("#8fbd5f"),
        QColor("#555555"),
        QColor("#ffffff")
    };

    return palette;
}