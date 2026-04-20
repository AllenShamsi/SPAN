#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <vector>
#include <limits>

// QCustomPlot
#include "qcustomplot.h"

// Forward decls for Qt events
class QMouseEvent;
class QCPAbstractItem;

//
// Minimal helper subclass used by label.cpp for the draggable handle.
//
class MyQCPItemText : public QCPItemText
{
public:
    explicit MyQCPItemText(QCustomPlot *parentPlot)
        : QCPItemText(parentPlot) {}
};

class Label : public QObject
{
    Q_OBJECT
public:
    explicit Label(QCustomPlot *plot);

    // External API used by mainWindow.cpp
    void placeLabelAt(double x, QString labelName);
    void removeLabelAt(double x, const QString &legendName);
    void removeAllLabels();

    void clearSelectedLine();
    bool selectLineIfMatch(const QString &legendName, double xOffset);

    void moveHandleBy(QCPItemText *handle, double deltaX);
    void updateLabelName(double x, const QString &actualName);

    QCustomPlot* getPlot() const { return m_plot; }

    bool isHandle(const QCPItemText* item) const;
    QCPItemLine* lineForHandle(const QCPItemText* item) const;

    // Snap MaxC to the nearest local extremum in the displacement trace
    double findNearestPeak(double xClicked, const QString &labelName);

signals:
    // Emitted when a label is created/removed/moved or clicked
    void landmarkAdded(const QString &channelName, double offset, const QString &labelName);
    void landmarkRemoved(double offset);
    void labelClicked(const QString &channelName, double offset);
    void labelMoved(const QString &channelName, const QString &labelName,
                    double newX, double oldX);

private slots:
    // Mouse interaction wiring
    void onItemClicked(QCPAbstractItem *item, QMouseEvent *event);
    void onMousePress(QMouseEvent *event);
    void onMouseMove(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);

private:
    // Small utilities used internally
    int countTrailingLowerCase(const QString &str);

    struct landmarks {
        double GONS  = std::numeric_limits<double>::quiet_NaN();
        double NONS  = std::numeric_limits<double>::quiet_NaN();
        double PVEL1 = std::numeric_limits<double>::quiet_NaN();
        double PVEL2 = std::numeric_limits<double>::quiet_NaN();
        double NOFFS = std::numeric_limits<double>::quiet_NaN();
        double GOFFS = std::numeric_limits<double>::quiet_NaN();
        double MaxC  = std::numeric_limits<double>::quiet_NaN();
    };

    void findPrecedingVelocityExtremumFromVector(landmarks &lm,
                                                 double xCenterSec,
                                                 const std::vector<double> &velocityData,
                                                 double sr);
    void findFollowingVelocityExtremumFromVector(landmarks &lm,
                                                 double xCenterSec,
                                                 const std::vector<double> &velocityData,
                                                 double sr);

private:
    QCustomPlot *m_plot{nullptr};

    // One vertical line per label
    QList<QCPItemLine*> m_labelLines;

    // Bottom numeric texts (offset display)
    QList<QCPItemText*> m_labelTexts;

    // Map: top-handle (text block) -> its line
    QMap<QCPItemText*, QCPItemLine*> m_labelHandleMapText;

    // Map: line -> its bottom numeric text (for quick updates)
    QMap<QCPItemLine*, QCPItemText*> m_lineToBottomText;

    // Bookkeeping
    QMap<QCPItemLine*, QString> m_labelLineLegendMap; // line -> series name (graph name)
    QMap<QCPItemLine*, QString> m_labelLineNameMap;   // line -> label name (e.g., "MaxC")

    // Selection/dragging state
    QCPItemLine  *m_currentlySelectedLine{nullptr};
    QCPItemText  *m_draggedHandle{nullptr};
    QCPItemLine  *m_draggedLine{nullptr};
    bool          m_isDragging{false};
    double        m_initialClickX{0.0};
    double        m_initialLineX{0.0};
};
