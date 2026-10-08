#ifndef AIRQ_CONSOLE_ALERT_OBSERVER_H
#define AIRQ_CONSOLE_ALERT_OBSERVER_H

#include "core/ServerObserver.h"

#include <cstddef>
#include <ostream>

// Observateur utilise par la version console : il affiche
// chaque alerte et les compte pour le resume a la fin.
class ConsoleAlertObserver : public ServerObserver {
public:
    explicit ConsoleAlertObserver(std::ostream& out);

    void onNotify(const Notification& notification) override;

    std::size_t getAlertCount() const;

private:
    std::ostream& out;
    std::size_t   alertCount;
};

#endif // AIRQ_CONSOLE_ALERT_OBSERVER_H
