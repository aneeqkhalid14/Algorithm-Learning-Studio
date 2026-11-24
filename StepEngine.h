#ifndef STEPENGINE_H
#define STEPENGINE_H

#include <QObject>
#include <QTimer>

// StepEngine is the single reusable component behind every "Previous / Next /
// Play / Pause / Reset" control bar in the app. It only tracks WHICH step is
// currently selected (an index from 0..totalSteps-1) and handles the timer
// for auto-play - it does not store the step data itself.
//
// Why: every topic's step data looks different (an array snapshot for
// sorting, a tree shape for the BST, a call stack for recursion, ...), so
// each topic page keeps its own small array of step records and simply asks
// this engine "what index am I on?" whenever it needs to redraw.
class StepEngine : public QObject {
    Q_OBJECT
public:
    explicit StepEngine(QObject *parent = nullptr);

    void setStepCount(int count);   // call after generating a new run's steps
    void reset();                   // jump back to step 0
    void next();
    void previous();
    void play();
    void pause();
    void setSpeedMs(int msPerStep);

    bool isPlaying() const { return playing_; }
    bool atStart() const   { return current_ <= 0; }
    bool atEnd() const     { return current_ >= total_ - 1; }
    int currentIndex() const { return current_; }
    int totalSteps() const   { return total_; }

signals:
    void stepChanged(int index);
    void playbackFinished();

private slots:
    void onTimerTick();

private:
    int total_;
    int current_;
    bool playing_;
    QTimer *timer_;
};

#endif // STEPENGINE_H
