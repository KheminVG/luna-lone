#pragma once

#include <QWidget>
#include "DataTypes.h"

namespace Ui {
    class AudioItemWidget;
}

class AudioItemWidget : public QWidget 
{
    Q_OBJECT

public:
    explicit AudioItemWidget(int id, const QString &name, const std::vector<TagEntry> &tags, QWidget* parent = nullptr);
    ~AudioItemWidget();

private:
    Ui::AudioItemWidget* ui;
    QString audioName;
    int audioId;

signals:
    void assignTagsClicked(int audioId, const QString &audioName);
    void deleteAudioClicked(int audioId);

private slots:
    void onAssignTagsClicked();
    void onDeleteAudioClicked();

};