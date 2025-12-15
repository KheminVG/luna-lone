#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
{
    m_audio_output = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audio_output);

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AudioPlayer::echoPlaybackState);
    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioPlayer::durationChanged);
}

AudioPlayer::~AudioPlayer() {
    delete m_player, m_audio_output;
}

void AudioPlayer::select(const QString &filePath)
{
    m_player->setSource(QUrl::fromLocalFile(filePath));
}

void AudioPlayer::play()
{
    if (m_player->source().isEmpty()) return;
    m_player->play();
};

void AudioPlayer::pause()
{
    if (m_player->source().isEmpty()) return;
    m_player->pause();
};

void AudioPlayer::stop()
{
    if (m_player->source().isEmpty()) return;
    m_player->stop();
};

bool AudioPlayer::isPlaying() const
{
    return m_player->playbackState() == QMediaPlayer::PlayingState;
};

void AudioPlayer::echoPlaybackState(QMediaPlayer::PlaybackState state) 
{
    if (state == QMediaPlayer::PlayingState) { emit playbackStarted(); }
    else if (state == QMediaPlayer::PausedState) { emit playbackPaused(); }
    else if (state == QMediaPlayer::StoppedState) { emit playbackStopped(); }
}
