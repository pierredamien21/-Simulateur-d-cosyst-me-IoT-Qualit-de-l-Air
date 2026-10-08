#include "core/AnomalyDetectorModel.h"

#include <cmath>
#include <numeric>
#include <vector>

// ------------------------------------------------------------
// On regarde l'ecart entre deux mesures qui se suivent, pas la
// valeur elle-meme. Sinon quand le CO2 monte doucement (ce qui
// est normal), chaque nouvelle valeur paraissait "anormale".
// Un vrai pic, lui, fait un gros saut d'un coup.
// La moyenne et l'ecart-type sont calcules SANS le dernier ecart,
// sinon le pic tire lui-meme la moyenne et le score reste petit.
// ------------------------------------------------------------
double AnomalyDetectorModel::predict(const std::deque<double>& history) const
{
    const std::size_t n = history.size();
    if (n < MIN_SAMPLES) {
        return -1.0;
    }

    std::vector<double> diffs;
    diffs.reserve(n - 1);
    for (std::size_t i = 1; i < n; ++i) {
        diffs.push_back(history[i] - history[i - 1]);
    }

    const double lastDiff = diffs.back();
    diffs.pop_back();   // on enleve le dernier ecart (celui qu'on teste)

    const double count = static_cast<double>(diffs.size());
    const double mean  = std::accumulate(diffs.begin(), diffs.end(), 0.0) / count;

    double acc = 0.0;
    for (double d : diffs) {
        acc += (d - mean) * (d - mean);
    }
    const double stdDev = std::sqrt(acc / count);

    if (stdDev < 1e-6) {
        return -1.0;
    }

    return std::fabs(lastDiff - mean) / stdDev;
}

std::string AnomalyDetectorModel::getName() const
{
    return "AnomalyDetector";
}
