#ifndef AIRQ_SERVER_OBSERVER_H
#define AIRQ_SERVER_OBSERVER_H

#include <string>

// Une alerte envoyee par le serveur
struct Notification {
    std::string timestamp;   // heure simulee (ISO 8601)
    std::string category;    // "CO2", "ANOMALY", "ACTUATOR"
    std::string message;
};

// Pattern Observer : tout ce qui veut recevoir les alertes
// du serveur herite de cette classe et s'inscrit avec
// Server::attach(). Le serveur ne supprime pas ses observateurs,
// il faut penser a appeler detach() avant de les detruire.
class ServerObserver {
public:
    virtual ~ServerObserver() = default;

    virtual void onNotify(const Notification& notification) = 0;
};

#endif // AIRQ_SERVER_OBSERVER_H
