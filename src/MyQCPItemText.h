#ifndef MYQCPITEMTEXT_H
#define MYQCPITEMTEXT_H

#include "qcustomplot.h"

class MyQCPItemText : public QCPItemText {
public:
    // Inherit constructors from QCPItemText.
    using QCPItemText::QCPItemText;

    // Override selectTest to enlarge the clickable area.
    double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details = nullptr) const {
        double d = QCPItemText::selectTest(pos, onlySelectable, details);
        return (d < 5) ? 0 : d;
    }
};

#endif // MYQCPITEMTEXT_H
