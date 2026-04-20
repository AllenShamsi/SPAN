#ifndef OPTIONMANAGER_H
#define OPTIONMANAGER_H

#include <QObject>
#include <QVector>
#include <QPair>

class OptionManager : public QObject {
    Q_OBJECT

public:
    OptionManager(QObject *parent = nullptr);
    void setOptionAvailability(const QString &option, bool available);
    bool isOptionAvailable(const QString &option) const;
    const QVector<QPair<QString, bool>>& getOptions() const;

signals:
    void optionAvailabilityChanged(const QString &option, bool available);

private:
    QVector<QPair<QString, bool>> options;  // Store options and their availability
};

#endif // OPTIONMANAGER_H

