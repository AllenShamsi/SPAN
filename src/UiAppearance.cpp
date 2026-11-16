#include "UiAppearance.h"
#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOption>
#include <QScreen>
#include <QStyleFactory>
#include <QStyleHints>
#include <QWidget>

namespace {
// Keep Fusion's check marks, tri-state marks, hover and focus rendering, but
// give the indicator boundary a predictable contrast against light surfaces.
class LightFusionStyle final : public QProxyStyle {
public:
    LightFusionStyle() : QProxyStyle(QStyleFactory::create(QStringLiteral("Fusion")))
    { setObjectName(QStringLiteral("fusion")); }
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget = nullptr) const override
    {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        if (element != PE_IndicatorCheckBox && element != PE_IndicatorRadioButton)
            return;
        painter->save();
        painter->setPen(QPen(option->palette.color(QPalette::Dark), 1));
        painter->setBrush(Qt::NoBrush);
        const QRectF border = QRectF(option->rect).adjusted(.5, .5, -.5, -.5);
        if (element == PE_IndicatorRadioButton) {
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawEllipse(border);
        } else {
            painter->drawRoundedRect(border, 1, 1);
        }
        painter->restore();
    }
};
QPalette lightPalette()
{
    QPalette p;
    for (auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
        const bool disabled = group == QPalette::Disabled;
        const QColor text(disabled ? "#666666" : "#202020");
        p.setColor(group, QPalette::Window, QColor("#f3f3f3"));
        p.setColor(group, QPalette::WindowText, text);
        p.setColor(group, QPalette::Base, QColor(disabled ? "#f3f3f3" : "#ffffff"));
        p.setColor(group, QPalette::AlternateBase, QColor("#f5f5f5"));
        p.setColor(group, QPalette::Text, text);
        p.setColor(group, QPalette::Button, QColor(disabled ? "#e9e9e9" : "#f0f0f0"));
        p.setColor(group, QPalette::ButtonText, text);
        p.setColor(group, QPalette::Light, Qt::white);
        p.setColor(group, QPalette::Midlight, QColor("#dadada"));
        p.setColor(group, QPalette::Mid, QColor("#909090"));
        p.setColor(group, QPalette::Dark, QColor("#666666"));
        p.setColor(group, QPalette::Shadow, QColor("#404040"));
        p.setColor(group, QPalette::BrightText, Qt::white);
        p.setColor(group, QPalette::Highlight, QColor(disabled ? "#b8cbe0" : "#1663a6"));
        p.setColor(group, QPalette::HighlightedText, QColor(disabled ? "#303030" : "#ffffff"));
        p.setColor(group, QPalette::ToolTipBase, Qt::white);
        p.setColor(group, QPalette::ToolTipText, QColor("#202020"));
        p.setColor(group, QPalette::Link, QColor("#005ea8"));
        p.setColor(group, QPalette::LinkVisited, QColor("#67418d"));
        p.setColor(group, QPalette::PlaceholderText, QColor("#666666"));
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        p.setColor(group, QPalette::Accent, QColor("#1663a6"));
#endif
    }
    return p;
}
}

void UiAppearance::apply(QApplication &app)
{
    // Fusion owns indicator/focus rendering; keep native file dialogs enabled.
    app.setStyle(new LightFusionStyle);
#ifdef Q_OS_WIN
    app.setFont(QFont(QStringLiteral("Segoe UI"), 9));
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    app.styleHints()->setColorScheme(Qt::ColorScheme::Light);
    QObject::connect(app.styleHints(), &QStyleHints::colorSchemeChanged, &app,
                     [&app](Qt::ColorScheme scheme) {
        if (scheme != Qt::ColorScheme::Light) {
            app.styleHints()->setColorScheme(Qt::ColorScheme::Light);
            app.setPalette(lightPalette());
        }
    });
#endif
    app.setPalette(lightPalette());
}

void UiAppearance::fitToScreen(QWidget &window)
{
    if (QScreen *screen = window.screen()) {
        const QSize available = screen->availableGeometry().size() - QSize(24, 48);
        window.resize(window.size().boundedTo(available.expandedTo(QSize(1, 1))));
    }
}
