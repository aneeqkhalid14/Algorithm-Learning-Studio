#include "Recursion.h"

namespace {

// Shared recording context threaded through the real recursive calls by
// reference - this is what lets a normal recursive function double as a
// step-by-step visualization source.
struct RecContext {
    QString frames[RECURSION_MAX_FRAMES];
    int frameCount = 0;
    RecursionStep *out = nullptr;
    int count = 0;
    int capacity = 0;
    OperationStats stats;

    void record(const QString &desc, const QString &returnText = QString())
    {
        if (count >= capacity) return;
        RecursionStep &s = out[count];
        s.frameCount = frameCount;
        for (int i = 0; i < frameCount; ++i) s.frames[i] = frames[i];
        s.description = desc;
        s.returnValueText = returnText;
        s.stats = stats;
        ++count;
    }

    void push(const QString &frame)
    {
        if (frameCount < RECURSION_MAX_FRAMES) frames[frameCount++] = frame;
    }

    void pop()
    {
        if (frameCount > 0) --frameCount;
    }
};

long long factorialHelper(int n, RecContext &ctx)
{
    ctx.stats.recursiveCalls++;
    QString frame = QString("factorial(%1)").arg(n);
    ctx.push(frame);
    ctx.record(QString("Calling %1 - pushed onto the call stack.").arg(frame));

    long long result;
    if (n <= 1) {
        result = 1;
        ctx.record(QString("Base case reached: %1 = 1.").arg(frame), QString::number(result));
    } else {
        long long sub = factorialHelper(n - 1, ctx);
        result = (long long)n * sub;
        ctx.record(QString("%1 computes %2 * %3 = %4.").arg(frame).arg(n).arg(sub).arg(result), QString::number(result));
    }
    ctx.pop();
    ctx.record(QString("Returning from %1 with value %2 - popped off the call stack.").arg(frame).arg(result), QString::number(result));
    return result;
}

long long fibonacciHelper(int n, RecContext &ctx)
{
    ctx.stats.recursiveCalls++;
    QString frame = QString("fib(%1)").arg(n);
    ctx.push(frame);
    ctx.record(QString("Calling %1 - pushed onto the call stack.").arg(frame));

    long long result;
    if (n <= 1) {
        result = n;
        ctx.record(QString("Base case reached: %1 = %2.").arg(frame).arg(result), QString::number(result));
    } else {
        long long a = fibonacciHelper(n - 1, ctx);
        long long b = fibonacciHelper(n - 2, ctx);
        result = a + b;
        ctx.record(QString("%1 computes fib(%2) + fib(%3) = %4 + %5 = %6.")
                       .arg(frame).arg(n - 1).arg(n - 2).arg(a).arg(b).arg(result), QString::number(result));
    }
    ctx.pop();
    ctx.record(QString("Returning from %1 with value %2 - popped off the call stack.").arg(frame).arg(result), QString::number(result));
    return result;
}

} // namespace

int Recursion::factorial(int n, RecursionStep *outSteps, int capacity)
{
    RecContext ctx;
    ctx.out = outSteps; ctx.capacity = capacity;
    ctx.record(QString("Computing factorial(%1).").arg(n));
    long long result = factorialHelper(n, ctx);
    ctx.record(QString("Final result: factorial(%1) = %2.").arg(n).arg(result));
    return ctx.count;
}

int Recursion::fibonacci(int n, RecursionStep *outSteps, int capacity)
{
    RecContext ctx;
    ctx.out = outSteps; ctx.capacity = capacity;
    ctx.record(QString("Computing fib(%1).").arg(n));
    long long result = fibonacciHelper(n, ctx);
    ctx.record(QString("Final result: fib(%1) = %2.").arg(n).arg(result));
    return ctx.count;
}
