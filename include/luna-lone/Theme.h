#pragma once
#include <QColor>

struct AccentPalette {
    QColor primary;
    QColor secondary;
    QColor danger;
    QColor succes;
    QColor neutral;
    QColor text;
};

class Theme {
public:
    static const AccentPalette& accents();    
};
