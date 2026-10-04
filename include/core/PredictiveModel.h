#ifndef AP4A_PREDICTIVE_MODEL_H
#define AP4A_PREDICTIVE_MODEL_H

#include <deque>
#include <string>

class PredictiveModel {
public:
    virtual ~PredictiveModel() = default;

    virtual double      predict(const std::deque<double>& history) const = 0;
    virtual std::string getName() const = 0;
};

#endif // AP4A_PREDICTIVE_MODEL_H