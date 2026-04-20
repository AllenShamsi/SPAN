/****************************************************************************
** Meta object code from reading C++ file 'QRangeSlider.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QRangeSlider.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRangeSlider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12QRangeSliderE_t {};
} // unnamed namespace

template <> constexpr inline auto QRangeSlider::qt_create_metaobjectdata<qt_meta_tag_ZN12QRangeSliderE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QRangeSlider",
        "minimumChange",
        "",
        "minimum",
        "maximumChange",
        "maximum",
        "lowValueChange",
        "lowValue",
        "highValueChange",
        "highValue",
        "rangeChange",
        "setMinimum",
        "setMaximum",
        "setLowValue",
        "setHighValue",
        "setRange"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'minimumChange'
        QtMocHelpers::SignalData<void(unsigned int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 3 },
        }}),
        // Signal 'maximumChange'
        QtMocHelpers::SignalData<void(unsigned int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 5 },
        }}),
        // Signal 'lowValueChange'
        QtMocHelpers::SignalData<void(unsigned int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 7 },
        }}),
        // Signal 'highValueChange'
        QtMocHelpers::SignalData<void(unsigned int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 9 },
        }}),
        // Signal 'rangeChange'
        QtMocHelpers::SignalData<void(unsigned int, unsigned int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 7 }, { QMetaType::UInt, 9 },
        }}),
        // Slot 'setMinimum'
        QtMocHelpers::SlotData<void(const unsigned int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 3 },
        }}),
        // Slot 'setMaximum'
        QtMocHelpers::SlotData<void(const unsigned int)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 5 },
        }}),
        // Slot 'setLowValue'
        QtMocHelpers::SlotData<void(const unsigned int)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 7 },
        }}),
        // Slot 'setHighValue'
        QtMocHelpers::SlotData<void(const unsigned int)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 9 },
        }}),
        // Slot 'setRange'
        QtMocHelpers::SlotData<void(const unsigned int, const unsigned int)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 3 }, { QMetaType::UInt, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QRangeSlider, qt_meta_tag_ZN12QRangeSliderE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QRangeSlider::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QRangeSliderE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QRangeSliderE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12QRangeSliderE_t>.metaTypes,
    nullptr
} };

void QRangeSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QRangeSlider *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->minimumChange((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 1: _t->maximumChange((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 2: _t->lowValueChange((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 3: _t->highValueChange((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 4: _t->rangeChange((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<uint>>(_a[2]))); break;
        case 5: _t->setMinimum((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 6: _t->setMaximum((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 7: _t->setLowValue((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 8: _t->setHighValue((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 9: _t->setRange((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<uint>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QRangeSlider::*)(unsigned int )>(_a, &QRangeSlider::minimumChange, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QRangeSlider::*)(unsigned int )>(_a, &QRangeSlider::maximumChange, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QRangeSlider::*)(unsigned int )>(_a, &QRangeSlider::lowValueChange, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (QRangeSlider::*)(unsigned int )>(_a, &QRangeSlider::highValueChange, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (QRangeSlider::*)(unsigned int , unsigned int )>(_a, &QRangeSlider::rangeChange, 4))
            return;
    }
}

const QMetaObject *QRangeSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QRangeSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QRangeSliderE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QRangeSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void QRangeSlider::minimumChange(unsigned int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QRangeSlider::maximumChange(unsigned int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void QRangeSlider::lowValueChange(unsigned int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void QRangeSlider::highValueChange(unsigned int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void QRangeSlider::rangeChange(unsigned int _t1, unsigned int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}
QT_WARNING_POP
