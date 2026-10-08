#include "core/DataAnalyzer.h"

#include <algorithm>   // std::transform, std::min_element, std::max_element
#include <cctype>      // std::tolower
#include <cmath>       // std::sqrt, std::fabs
#include <numeric>     // std::accumulate

// Constructeur
DataAnalyzer::DataAnalyzer() : history() {}

// Normalisation de cle ("CO2" -> "co2")
std::string DataAnalyzer::normalize(const std::string& type) {
    std::string key = type;
    std::transform(key.begin(), key.end(), key.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
    return key;
}

// Alimentation (fenetre glissante)
void DataAnalyzer::push(const std::string& type, double value) {
    auto& dq = history[normalize(type)];
    dq.push_back(value);

    if (dq.size() > WINDOW_SIZE) {
        dq.pop_front();
    }
}

// Vide tout (bouton Reset)
void DataAnalyzer::clear() {
    history.clear();
}

// Moyenne
double DataAnalyzer::getAverage(const std::string& type) const {
    auto it = history.find(normalize(type));
    if (it == history.end() || it->second.empty()) {
        return 0.0;
    }

    const auto& dq = it->second;
    const double sum = std::accumulate(dq.begin(), dq.end(), 0.0);
    return sum / static_cast<double>(dq.size());
}

// Ecart-type (population)
double DataAnalyzer::getStdDev(const std::string& type) const {
    auto it = history.find(normalize(type));
    if (it == history.end() || it->second.size() < 2) {
        return 0.0;
    }

    const double mean = getAverage(type);
    double acc = 0.0;
    for (double v : it->second) {
        acc += (v - mean) * (v - mean);
    }
    return std::sqrt(acc / static_cast<double>(it->second.size()));
}

// Minimum / Maximum
double DataAnalyzer::getMin(const std::string& type) const {
    auto it = history.find(normalize(type));
    if (it == history.end() || it->second.empty()) {
        return 0.0;
    }
    return *std::min_element(it->second.begin(), it->second.end());
}

double DataAnalyzer::getMax(const std::string& type) const {
    auto it = history.find(normalize(type));
    if (it == history.end() || it->second.empty()) {
        return 0.0;
    }
    return *std::max_element(it->second.begin(), it->second.end());
}

// Derniere valeur de la fenetre (valeur brute la plus recente)
double DataAnalyzer::getLast(const std::string& type) const {
    auto it = history.find(normalize(type));
    if (it == history.end() || it->second.empty()) {
        return 0.0;
    }
    return it->second.back();
}

// Historique (copie)
std::deque<double> DataAnalyzer::getHistory(const std::string& type) const {
    auto it = history.find(normalize(type));
    if (it == history.end()) {
        return {};
    }
    return it->second;
}

// AQI (0..100)
double DataAnalyzer::computeAQI() const {
    // --- CO2 (400 ppm = parfait, 1500 ppm = mauvais) ---
    const double co2 = getAverage("co2");
    double co2Score = 100.0;
    if (co2 > 400.0) {
        co2Score = 100.0 - (co2 - 400.0) / (1500.0 - 400.0) * 100.0;
    }
    co2Score = std::max(0.0, std::min(100.0, co2Score));

    // --- Temperature (ideal ~22 C, tolerance +-3 C) ---
    const double temp = getAverage("temperature");
    double tempScore = 100.0;
    if (temp > 0.0) {
        const double delta = std::fabs(temp - 22.0);
        tempScore = std::max(0.0, 100.0 - delta * 20.0);
    }

    // --- Humidite (ideal ~50 %, tolerance +-10 %) ---
    const double hum = getAverage("humidity");
    double humScore = 100.0;
    if (hum > 0.0) {
        const double delta = std::fabs(hum - 50.0);
        humScore = std::max(0.0, 100.0 - delta * 5.0);
    }

    // --- Ponderation ---
    return co2Score * 0.6 + tempScore * 0.2 + humScore * 0.2;
}

// Accesseurs
std::size_t DataAnalyzer::getSampleCount(const std::string& type) const {
    auto it = history.find(normalize(type));
    return (it == history.end()) ? 0 : it->second.size();
}

std::size_t DataAnalyzer::getTypeCount() const {
    return history.size();
}