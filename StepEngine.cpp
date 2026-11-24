#include "StepEngine.h"

StepEngine::StepEngine(QObject *parent)
    : QObject(parent), total_(0), current_(-1), playing_(false)
{
    timer_ = new QTimer(this);
    timer_->setInterval(700);
    connect(timer_, &QTimer::timeout, this, &StepEngine::onTimerTick);
}

void StepEngine::setStepCount(int count)
{
    pause();
    total_ = count;
    current_ = (count > 0) ? 0 : -1;
    emit stepChanged(current_);
}

void StepEngine::reset()
{
    pause();
    current_ = (total_ > 0) ? 0 : -1;
    emit stepChanged(current_);
}

void StepEngine::next()
{
    if (current_ < total_ - 1) {
        ++current_;
        emit stepChanged(current_);
    } else {
        pause();
        emit playbackFinished();
    }
}

void StepEngine::previous()
{
    if (current_ > 0) {
        --current_;
        emit stepChanged(current_);
    }
}

void StepEngine::play()
{
    if (total_ == 0) return;
    if (atEnd()) reset();
    playing_ = true;
    timer_->start();
}

void StepEngine::pause()
{
    playing_ = false;
    timer_->stop();
}

void StepEngine::setSpeedMs(int msPerStep)
{
    timer_->setInterval(msPerStep);
}

void StepEngine::onTimerTick()
{
    if (atEnd()) {
        pause();
        emit playbackFinished();
        return;
    }
    next();
}
