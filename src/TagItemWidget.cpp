#include "ui_TagItemWidget.h"
#include "TagItemWidget.h"

TagItemWidget::TagItemWidget(int id, const QString &name, QWidget* parent)
    : QWidget(parent), tagId(id), tagName(name), ui(new Ui::TagItemWidget)
{
    ui->setupUi(this);
    
    ui->nameLabel->setText(tagName);

    connect(ui->deleteButton, &QPushButton::clicked, this, &TagItemWidget::onDeleteTagClicked);
}

TagItemWidget::~TagItemWidget() 
{
    delete ui;
}

void TagItemWidget::onDeleteTagClicked() 
{
    emit deleteTagClicked(tagId);
}
