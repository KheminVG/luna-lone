#include "ui_TagChip.h"
#include "TagChip.h"

TagChip::TagChip(int id, const QString &name, QWidget* parent, bool toggleable)
    : QWidget(parent), tagId(id), tagName(name), ui(new Ui::TagChip)
{
    ui->setupUi(this);
    
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
    if (selected) {
        ui->tagButton->setStyleSheet("background-color: rgba(102, 53, 153, 1); border-radius: 10px;");
    } else {
        ui->tagButton->setStyleSheet("background-color: rgba(70, 70, 70, 1); border-radius: 10px;");
    }
}

void TagChip::toggle()
{
    setSelected(!selected);
}
