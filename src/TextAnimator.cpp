#include "TextAnimator.h"

TextAnimator::TextAnimator(QTextBrowser *target, QObject *parent)
    : QObject(parent), m_target(target), m_index(0)
{
    connect(&m_timer, &QTimer::timeout, this, &TextAnimator::typeNextChar);
}

void TextAnimator::startTyping(const QString &text, int interval_ms)
{
    m_full_text = text;
    m_index = 0;

    m_target->clear();
    m_timer.start(interval_ms);
}

void TextAnimator::stop()
{
    m_timer.stop();
}

void TextAnimator::typeNextChar()
{
    if (m_index >= m_full_text.size()) {
        stop();
        return;
    }

    m_target->insertPlainText(QString(m_full_text[m_index]));
    m_index++;
}
