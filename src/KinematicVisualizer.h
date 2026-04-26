#ifndef KINEMATICVISUALIZER_H
#define KINEMATICVISUALIZER_H

#include <QWidget>
#include <QHash>
#include <QVector>
#include <QPair>
#include "qcustomplot.h"
#include "label.h"

class KinematicVisualizer : public QWidget {
    Q_OBJECT
public:
    explicit KinematicVisualizer(QWidget *parent = nullptr);
    ~KinematicVisualizer();

    // Basic accessors
    QCustomPlot* getCustomPlot() const { return customPlot; }
    Label*       getLabel()     const { return m_label;     }

    // Rendering
    void visualizeSignal(const QMap<QString, QVector<double>> &dataMap,
                                              const QString &configName,
                                              double penWidth,
                         int samplingRate);

    void visualizeSpectrogram(const QVector<QVector<double>> &spectrogramData,
                              const QString &configName,
                              double tStart,
                              double tEnd,
                              double maxFrequency,
                              double dynamicRangeDb);

    // Called by mainWindow when the X-view changes
    void updateSpectrogramColorScaleForRange(const QCPRange &visibleRange);

    // Data helpers
    void setTrackedParameter(const QString &parameter);
    void setSignalData(const QMap<QString, QVector<double>> &dataMap);

    // Selection helpers
    static void clearSelectionRect();
    QCPRange getSelectionRange() const;

    // Ranges
    double getXAxisMinLimit() const { return xAxisMinLimit; }
    double getXAxisMaxLimit() const { return xAxisMaxLimit; }

    // Visual config
    void setConfigName (const QString &name) { m_configName = name; }
    QString configName() const { return m_configName; }
    void setBaseColor(const QColor &c) { m_baseColor = c; }

    double getSignalValueAt(double x, QString *usedAxis = nullptr) const;

public slots:
    void zoomToSelection();

protected:
    // Qt overrides
    bool eventFilter(QObject *object, QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    // Sync x-range across plots
    void synchronizePlots(const QCPRange &newRange);

    // Mouse/selection flow
    void onMouseRelease();
    void onMouseDrag();
    void onAnyMousePress(QMouseEvent *event);

private:
    // ----- One plot per instance -----
    QCustomPlot *customPlot = nullptr;

    // Cursor overlay on the "main" plot of this instance
    QCPItemLine *hLine      = nullptr;  // horizontal cursor
    QCPItemText *coordText  = nullptr;  // "X: ..., Y: ..."
    QCPItemRect *coordFrame = nullptr;  // background for coordText

    // Label widget (handles drag handles etc.)
    Label *m_label = nullptr;

    // Instance state
    QString m_configName;
    QColor m_baseColor;

    QPoint  cursorPos{};
    bool    selecting = false;

    double  xAxisMinLimit = 0.0;
    double  xAxisMaxLimit = 0.0;

    // For value readouts under cursor
    QVector<QPair<double,double>> signalDataX;
    QVector<QPair<double,double>> signalDataY;
    QVector<QPair<double,double>> signalDataZ;
    QString trackedParameter;                    // "X", "Y", or "Z"
    QHash<QString, double> signalOffsets;         // align channels around global center

    // Selection interaction state
    QCPItemText* m_clickedHandle = nullptr;      // which label-handle was pressed
    double       m_pressX = 0.0;                 // press x in plot coords
    QPoint       m_pressPos;                     // press position in pixels

    // Setup helpers
    void setupCustomPlot();
    void setupCursorItems(QCustomPlot *plot);

    // Cursor/line updates
    void updateCursorItems(QCustomPlot *plot);
    void hideHorizontalCursor();
    void hideAllVerticalLines();
    void updateVerticalLineInAllPlots(double x);

    // Data helpers
    double getYValueFromSignal(double x, QString *usedAxis = nullptr);

    // X-limits helpers (kept for future use / API symmetry)
    void setZoomLimits(double minLimit, double maxLimit);
    bool isAtZoomOutLimit();

    // Spectrogram support
    bool m_isSpectrogram = false;
    QVector<QVector<double>> m_spectrogramData;
    double m_spectrogramDuration = 0.0;
    QCPColorMap* m_spectrogramColorMap = nullptr;

    double m_spectrogramDynRangeDb { 70.0 };
    double m_spectrogramStartTime { 0.0 };


    // Audio auto-scale
    QVector<QPair<double,double>> m_audioSignalFullRes;
    bool m_autoScaleAudioY = false;

    void rescaleAudioYAxisToVisibleRange(const QCPRange &range);


    // Static/shared across all visualizer instances
private:
    // Shared across all instances
    static QList<QCustomPlot*>                          customPlots;
    static QHash<QCustomPlot*, QCPItemLine*>            vLinesMap;
    static QCustomPlot*                                 lastPlotWithLine;
    static QCPItemRect*                                 selectionRect;

    static QHash<QCustomPlot*, QCPItemRect*>            globalSelectionRects;
    static QHash<QCustomPlot*, QCPItemLine*>            globalSelectionLeftLines;
    static QHash<QCustomPlot*, QCPItemLine*>            globalSelectionRightLines;
    static QHash<QCustomPlot*, QCPItemText*>            globalSelectionLeftLabels;
    static QHash<QCustomPlot*, QCPItemText*>            globalSelectionRightLabels;
    static QHash<QCustomPlot*, QCPItemText*>            globalSelectionDistanceLabels;

    // Optional: sync Y across all plots (utility kept; not auto-invoked)
    static void synchronizeYAxes();
};

#endif // KINEMATICVISUALIZER_H
