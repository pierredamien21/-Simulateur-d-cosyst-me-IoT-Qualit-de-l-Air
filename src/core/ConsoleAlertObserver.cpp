#include "core/ConsoleAlertObserver.h"

ConsoleAlertObserver::ConsoleAlertObserver(std::ostream& o)
    : out(o)
    , alertCount(0)
{
}

void ConsoleAlertObserver::onNotify(const Notification& n)
{
    ++alertCount;
    out << "    [ALERTE " << n.timestamp << "] "
        << n.category << " : " << n.message << "\n";
}

std::size_t ConsoleAlertObserver::getAlertCount() const
{
    return alertCount;
}
