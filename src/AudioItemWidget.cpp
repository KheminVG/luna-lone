#include "ui_AudioItemWidget.h"
#include "AudioItemWidget.h"
#include "TagChip.h"

AudioItemWidget::AudioItemWidget(int id, const QString &name, const std::vector<TagEntry> &tags, QWidget* parent)
    : QWidget(parent), audioId(id), audioName(name), ui(new Ui::AudioItemWidget)
{
    ui->setupUi(this);
    
    ui->nameLabel->setText(audioName);

    for (const auto &tag : tags) {
        TagChip* tagChip = new TagChip(tag.id, QString::fromStdString(tag.tagName), this, false);
        ui->tagsContainer->addWidget(tagChip);
    }

    connect(ui->assignTagsButton, &QPushButton::clicked, this, &AudioItemWidget::onAssignTagsClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &AudioItemWidget::onDeleteAudioClicked);
}

AudioItemWidget::~AudioItemWidget() 
{
    delete ui;
}

void AudioItemWidget::onAssignTagsClicked() 
{
    emit assignTagsClicked(audioId, audioName);
}

void AudioItemWidget::onDeleteAudioClicked() 
{
    emit deleteAudioClicked(audioId);
}
