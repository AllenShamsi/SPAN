#include "OptionManager.h"

OptionManager::OptionManager(QObject *parent)
    : QObject(parent)
{
    options.push_back(QPair<QString, bool>("Select", false));
    options.append({"REF-L", true});
    options.append({"REF-R", true});
    options.append({"REF-F", true});
    options.append({"J", true});
    options.append({"TT", true});
    options.append({"TM", true});
    options.append({"TD", true});
    options.append({"UL", true});
    options.append({"LL", true});
}

void OptionManager::setOptionAvailability(const QString &option, bool available)
{
    for (auto &opt : options) {
        if (opt.first == option) {
            if (opt.second != available) {
                opt.second = available;
                emit optionAvailabilityChanged(option, available);
            }
            break;
        }
    }
}

bool OptionManager::isOptionAvailable(const QString &option) const
{
    for (const auto &opt : options) {
        if (opt.first == option) {
            return opt.second;
        }
    }
    return false;
}

const QVector<QPair<QString, bool>>& OptionManager::getOptions() const
{
    return options;
}
