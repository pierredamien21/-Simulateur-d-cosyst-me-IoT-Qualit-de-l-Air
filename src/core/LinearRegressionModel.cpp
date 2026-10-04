#include "core/LinearRegressionModel.h"

#include <cmath>

double LinearRegressionModel::predict(const std::deque<double>& history) const
{
    const std::size_t n = history.size();
    if (n < MIN_SAMPLES) {
        return -1.0;
    }

    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        const double x = static_cast<double>(i);
        const double y = history[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }

    const double nD = static_cast<double>(n);
    const double denom = nD * sumX2 - sumX * sumX;
    if (std::fabs(denom) < 1e-9) {
        return -1.0;
    }

    const double a = (nD * sumXY - sumX * sumY) / denom;
    const double b = (sumY - a * sumX) / nD;

    if (a <= 0.0) {
        return -1.0;
    }
    if (history.back() >= THRESHOLD) {
        return 0.0;
    }

    const double futureX = (THRESHOLD - b) / a;
    const double lastIndex = static_cast<double>(n - 1);
    const double samplesUntilThreshold = futureX - lastIndex;

    return (samplesUntilThreshold < 0.0) ? -1.0 : samplesUntilThreshold;
}

std::string LinearRegressionModel::getName() const
{
    return "LinearRegression";
}