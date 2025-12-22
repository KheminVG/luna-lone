#include "ui_TagChip.h"
#include "TagChip.h"
#include "Theme.h"

TagChip::TagChip(int id, const QString &name, QWidget* parent, bool toggleable)
    : QWidget(parent), tagId(id), tagName(name), ui(new Ui::TagChip)
{
    ui->setupUi(this);

    auto& a = Theme::accents();
    ui->tagButton->setStyleSheet(QString("color: %1;background-color: %2;border-radius: 10px;").arg(a.text.name(), a.primary.name()));
    
    ui->tagButton->setText(tagName);
    this->setFixedWidth(tagName.size() * 8 + 24);

    setToggleable(toggleable);
    updateStyle();
}

TagChip::~TagChip()
{
    delete ui;
}

void TagChip::setToggleable(bool toggleable)
{
    m_toggleable = toggleable;

    if (!m_toggleable) {
        ui->tagButton->setCheckable(false);
        ui->tagButton->setCursor(Qt::ArrowCursor);

        setSelected(true);
        disconnect(ui->tagButton, &QPushButton::clicked, this, &TagChip::toggle);
    } else {
        ui->tagButton->setCheckable(true);
        ui->tagButton->setCursor(Qt::PointingHandCursor);
        connect(ui->tagButton, &QPushButton::clicked, this, &TagChip::toggle);
    }
}

void TagChip::setSelected(bool value)
{
    if (selected != value) {
        selected = value;
        updateStyle();
        emit toggled(tagId, selected);
    }
}

void TagChip::updateStyle()
{
    auto& a = Theme::accents();
    
    if (selected) {
        ui->tagButton->setStyleSheet(QString("color: %1;background-color: %2;border-radius: 10px;").arg(a.text.name(), a.primary.name()));
    } else {
        ui->tagButton->setStyleSheet(QString("color: %1;background-color: %2;border-radius: 10px;").arg(a.text.name(), a.neutral.name()));
    }
}

void TagChip::toggle()
{
    setSelected(!selected);
}
