#include <QApplication>
#include <QIcon>
#include <QProxyStyle>
#include <QStyleFactory>
#include "mainWindow.h"

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
            return 0; // never auto-hide on mac
        return QProxyStyle::styleHint(hint, opt, w, ret);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icons/SPAN.jpg"));

#ifdef Q_OS_MAC
    // Must be set BEFORE any widgets are created
    QStyle *fusion = QStyleFactory::create("Fusion");
    app.setStyle(new NoTransientScrollBarsStyle(fusion));
#endif

    mainWindow w;
    w.show();
    return app.exec();
}
