#include <QApplication>
#include <QIcon>
#include <QProxyStyle>
#include <QStyleFactory>
#include "mainWindow.h"
#include <QStyleHints>
#include "UiAppearance.h"

class NoTransientScrollBarsStyle : public QProxyStyle {
public:
    explicit NoTransientScrollBarsStyle(QStyle *base)
        : QProxyStyle(base) {}

    int styleHint(StyleHint hint,
                  const QStyleOption *opt = nullptr,
                  const QWidget *w = nullptr,
                  QStyleHintReturn *ret = nullptr) const override
    {
        if (hint == QStyle::SH_ScrollBar_Transient)
            return 0; // Keep macOS scrollbars visible.
        return QProxyStyle::styleHint(hint, opt, w, ret);
    }
};

int main(int argc, char *argv[])
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication app(argc, argv);
    UiAppearance::apply(app);
    QCoreApplication::setApplicationName(QStringLiteral("SPAN"));
    QCoreApplication::setApplicationVersion(QStringLiteral(SPAN_VERSION));


#ifdef Q_OS_MAC
    QStyle *fusion = QStyleFactory::create("Fusion");
    app.setStyle(new NoTransientScrollBarsStyle(fusion));
#endif

    mainWindow w;
    UiAppearance::fitToScreen(w);
    w.show();
    return app.exec();
}
