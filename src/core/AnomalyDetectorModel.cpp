#include "core/AnomalyDetectorModel.h"

#include <cmath>
#include <numeric>

double AnomalyDetectorModel::predict(const std::deque<double>& history) const
{
    const std::size_t n = history.size();
    if (n < MIN_SAMPLES) {
        return -1.0;
    }

    const double sum = std::accumulate(history.begin(), history.end(), 0.0);
    const double mean = sum / static_cast<double>(n);

    double acc = 0.0;
    for (double v : history) {
        acc += (v - mean) * (v - mean);
    }
    const double stdDev = std::sqrt(acc / static_cast<double>(n));

    if (stdDev < 1e-6) {
        return -1.0;
    }

    const double last = history.back();
    return std::fabs(last - mean) / stdDev;
}

std::string AnomalyDetectorModel::getName() const
{
    return "AnomalyDetector";
}