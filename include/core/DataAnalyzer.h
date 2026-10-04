#ifndef AP4A_DATA_ANALYZER_H
#define AP4A_DATA_ANALYZER_H

#include <cstddef>
#include <deque>
#include <map>
#include <string>

/**
 * @brief Analyseur de donnees capteurs (fenetre glissante).
 *
 * Stocke les N dernieres valeurs par type de capteur et
 * fournit des statistiques : moyenne, ecart-type, min/max,
 * et un indice global de qualite de l'air (AQI).
 *
 * CONVENTION DE NOMMAGE :
 *   Toutes les cles de type sont normalisees en minuscules
 *   en interne ("CO2" -> "co2", "Temperature" -> "temperature").
 *   Cela garantit la coherence avec les noms de fichiers CSV
 *   produits par Server (via Sensor::logKey()).
 *
 * Utilise la STL : std::map + std::deque + std::accumulate.
 */
class DataAnalyzer {
public:
    static constexpr std::size_t WINDOW_SIZE = 20;

    DataAnalyzer();

    // Alimentation
    void push(const std::string& type, double value);

    // Statistiques (casse du type indifferente)
    double getAverage(const std::string& type) const;
    double getStdDev(const std::string& type) const;
    double getMin(const std::string& type) const;
    double getMax(const std::string& type) const;
    double getLast(const std::string& type) const;    // <-- AJOUT

    // Historique (copie du deque)
    std::deque<double> getHistory(const std::string& type) const;

    // Indice de qualite de l'air (0 = mauvais, 100 = excellent)
    double computeAQI() const;

    // Accesseurs
    std::size_t getSampleCount(const std::string& type) const;
    std::size_t getTypeCount() const;

private:
    // Normalise une cle de type ("CO2" -> "co2")
    static std::string normalize(const std::string& type);

    std::map<std::string, std::deque<double>> history;
};

#endif // AP4A_DATA_ANALYZER_H