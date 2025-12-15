#pragma once

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QString>

class AudioPlayer : public QObject {
    Q_OBJECT

public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    void select(const QString &filePath);
    void play();
    void pause();
    void stop();
    bool isPlaying() const;

signals:
    void playbackStarted();
    void playbackPaused();
    void playbackStopped();

    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audio_output;

private slots:
    void echoPlaybackState(QMediaPlayer::PlaybackState state);
};