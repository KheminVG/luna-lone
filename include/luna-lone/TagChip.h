#pragma once

#include <QWidget>

namespace Ui {
    class TagChip;
}

class TagChip : public QWidget {
    Q_OBJECT

public:
    explicit TagChip(int id, const QString &name, QWidget* parent = nullptr, bool toggleable = true);
    ~TagChip();

    void setToggleable(bool toggleable);
    bool isToggleable() const { return m_toggleable; }

public slots:
    void setSelected(bool value);

signals:
    void toggled(int tagId, bool selected);

private:
    void updateStyle();
    void toggle();

    Ui::TagChip* ui;
    QString tagName;
    int tagId;
    bool selected = false;
    bool m_toggleable;
};