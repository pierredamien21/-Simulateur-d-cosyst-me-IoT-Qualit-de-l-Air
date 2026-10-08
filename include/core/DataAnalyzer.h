#ifndef AIRQ_DATA_ANALYZER_H
#define AIRQ_DATA_ANALYZER_H

#include <cstddef>
#include <deque>
#include <map>
#include <string>

// Garde les 20 dernieres valeurs de chaque capteur et calcule
// des stats dessus (moyenne, ecart-type, min, max) + l'AQI.
// Les noms sont toujours passes en minuscules en interne, comme
// ca "CO2" et "co2" donnent la meme chose.
class DataAnalyzer {
public:
    static constexpr std::size_t WINDOW_SIZE = 20;

    DataAnalyzer();

    void push(const std::string& type, double value);
    void clear();

    // Majuscules / minuscules acceptees
    double getAverage(const std::string& type) const;
    double getStdDev(const std::string& type) const;
    double getMin(const std::string& type) const;
    double getMax(const std::string& type) const;
    double getLast(const std::string& type) const;

    // Renvoie une copie
    std::deque<double> getHistory(const std::string& type) const;

    // Qualite de l'air sur 100 (100 = parfait)
    double computeAQI() const;

    std::size_t getSampleCount(const std::string& type) const;
    std::size_t getTypeCount() const;

private:
    static std::string normalize(const std::string& type);

    std::map<std::string, std::deque<double>> history;
};

#endif // AIRQ_DATA_ANALYZER_H