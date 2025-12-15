#pragma once

#include <QMainWindow>
#include "AudioPlayer.h"
#include "TextAnimator.h"
#include "Database.h"
#include <memory>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    AudioPlayer* m_audio_player;
    TextAnimator* m_text_animator;

    std::shared_ptr<Database> database;
    QString selectedFilePath;

private slots:
    void onSelectFileClicked();
    void onAddAudioClicked();
    void onAddTagClicked();

    void onAssignTags(int audioId, const QString &audioName);
    void onDeleteAudio(int audioId);
    void onDeleteTag(int tagId);

    void showMainView();
    void showAudioListView();
    void showTagListView();

    void refreshAudioList();
    void refreshTagList();

    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onAudioPositionChanged(qint64 position);
    void onAudioDurationChanged(qint64 duration);

    void onPromptEntered();
    void processPrompt(const std::string &prompt);
};