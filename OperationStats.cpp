#include "OperationStats.h"

void OperationStats::reset()
{
    comparisons = 0;
    swaps = 0;
    arrayAccesses = 0;
    insertions = 0;
    deletions = 0;
    probes = 0;
    collisions = 0;
    recursiveCalls = 0;
}

QString OperationStats::summary() const
{
    QString text;
    if (comparisons)   text += QString("Comparisons: %1   ").arg(comparisons);
    if (swaps)         text += QString("Swaps: %1   ").arg(swaps);
    if (arrayAccesses) text += QString("Array accesses: %1   ").arg(arrayAccesses);
    if (insertions)    text += QString("Insertions: %1   ").arg(insertions);
    if (deletions)     text += QString("Deletions: %1   ").arg(deletions);
    if (probes)        text += QString("Probes: %1   ").arg(probes);
    if (collisions)    text += QString("Collisions: %1   ").arg(collisions);
    if (recursiveCalls)text += QString("Recursive calls: %1   ").arg(recursiveCalls);
    if (text.isEmpty()) text = "No operations recorded yet.";
    return text;
}
