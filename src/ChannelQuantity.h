#ifndef CHANNELQUANTITY_H
#define CHANNELQUANTITY_H

#include <QString>
#include <string>

// Presentation metadata only. Never serialized; no numerical data are changed.
struct ChannelQuantity {
    enum class Kind { Unspecified, Position, Difference, Distance };
    Kind kind = Kind::Unspecified;
    // Bits 0/1/2 identify meaningful X/Y/Z components (DIST stores X only).
    unsigned components = 0;

    static ChannelQuantity ag50xPosition() { return {Kind::Position, 7}; }
};

inline ChannelQuantity derivedChannelQuantity(ChannelQuantity a, ChannelQuantity b,
                                               const std::string& operation)
{
    if (a.kind == ChannelQuantity::Kind::Unspecified ||
        b.kind == ChannelQuantity::Kind::Unspecified) return {};
    if (operation == "SUB")
        return {ChannelQuantity::Kind::Difference, a.components & b.components};
    if (operation == "DIST")
        return {ChannelQuantity::Kind::Distance, 1};
    return {};
}

// derivativeOrder and selected components come from SensorConfig, never its name.
inline QString kinematicAxisTitle(ChannelQuantity quantity, int derivativeOrder,
                                   unsigned selectedComponents, bool inverted = false)
{
    using Kind = ChannelQuantity::Kind;
    if (quantity.kind == Kind::Unspecified || selectedComponents == 0 ||
        (selectedComponents & ~quantity.components) != 0 ||
        derivativeOrder < 0 || derivativeOrder > 2)
        return QStringLiteral("Value (unit unspecified)");

    const bool multiple = (selectedComponents & (selectedComponents - 1)) != 0;
    QString name;
    if (derivativeOrder == 0) {
        if (quantity.kind == Kind::Distance) name = QStringLiteral("Distance");
        else if (quantity.kind == Kind::Difference) name = QStringLiteral("Displacement difference");
        else name = QStringLiteral("Position");
    } else if (derivativeOrder == 1) {
        if (multiple) name = QStringLiteral("Tangential speed");
        else if (quantity.kind == Kind::Distance) name = QStringLiteral("Distance rate");
        else if (quantity.kind == Kind::Difference) name = QStringLiteral("Difference velocity");
        else name = QStringLiteral("Velocity");
    } else {
        if (multiple) name = QStringLiteral("Tangential acceleration");
        else if (quantity.kind == Kind::Distance) name = QStringLiteral("Distance second derivative");
        else if (quantity.kind == Kind::Difference) name = QStringLiteral("Difference acceleration");
        else name = QStringLiteral("Acceleration");
    }
    if (inverted) name.prepend(QLatin1Char('-'));
    QString unit = QStringLiteral("mm");
    if (derivativeOrder > 0) unit += QStringLiteral("/s");
    if (derivativeOrder == 2) unit += QChar(0x00b2);
    return name + QStringLiteral(" (") + unit + QLatin1Char(')');
}

#endif // CHANNELQUANTITY_H
