#pragma once

#include <QObject>
#include <QTextBrowser>
#include <QTimer>

class TextAnimator : public QObject {
    Q_OBJECT

public:
    TextAnimator(QTextBrowser *target, QObject *parent = nullptr);
    ~TextAnimator() = default;

    void startTyping(const QString &text, int interval_ms = 35);
    void stop();

private slots:
    void typeNextChar();

private:
    QTextBrowser *m_target;
    QString m_full_text;
    int m_index;
    QTimer m_timer;

};