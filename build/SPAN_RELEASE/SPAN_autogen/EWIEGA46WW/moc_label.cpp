/****************************************************************************
** Meta object code from reading C++ file 'label.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../label.h"
#include <QtGui/qscreen.h>
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'label.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN5LabelE_t {};
} // unnamed namespace

template <> constexpr inline auto Label::qt_create_metaobjectdata<qt_meta_tag_ZN5LabelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Label",
        "landmarkAdded",
        "",
        "channelName",
        "offset",
        "labelName",
        "landmarkRemoved",
        "labelClicked",
        "labelMoved",
        "newX",
        "oldX",
        "onItemClicked",
        "QCPAbstractItem*",
        "item",
        "QMouseEvent*",
        "event",
        "onMousePress",
        "onMouseMove",
        "onMouseRelease"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'landmarkAdded'
        QtMocHelpers::SignalData<void(const QString &, double, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Double, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'landmarkRemoved'
        QtMocHelpers::SignalData<void(double)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 4 },
        }}),
        // Signal 'labelClicked'
        QtMocHelpers::SignalData<void(const QString &, double)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Double, 4 },
        }}),
        // Signal 'labelMoved'
        QtMocHelpers::SignalData<void(const QString &, const QString &, double, double)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 5 }, { QMetaType::Double, 9 }, { QMetaType::Double, 10 },
        }}),
        // Slot 'onItemClicked'
        QtMocHelpers::SlotData<void(QCPAbstractItem *, QMouseEvent *)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 12, 13 }, { 0x80000000 | 14, 15 },
        }}),
        // Slot 'onMousePress'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Slot 'onMouseMove'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Slot 'onMouseRelease'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Label, qt_meta_tag_ZN5LabelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Label::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5LabelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5LabelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN5LabelE_t>.metaTypes,
    nullptr
} };

void Label::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Label *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->landmarkAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->landmarkRemoved((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->labelClicked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 3: _t->labelMoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[4]))); break;
        case 4: _t->onItemClicked((*reinterpret_cast<std::add_pointer_t<QCPAbstractItem*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[2]))); break;
        case 5: _t->onMousePress((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 6: _t->onMouseMove((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 7: _t->onMouseRelease((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAbstractItem* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Label::*)(const QString & , double , const QString & )>(_a, &Label::landmarkAdded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Label::*)(double )>(_a, &Label::landmarkRemoved, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Label::*)(const QString & , double )>(_a, &Label::labelClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Label::*)(const QString & , const QString & , double , double )>(_a, &Label::labelMoved, 3))
            return;
    }
}

const QMetaObject *Label::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Label::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5LabelE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Label::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Label::landmarkAdded(const QString & _t1, double _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void Label::landmarkRemoved(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void Label::labelClicked(const QString & _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void Label::labelMoved(const QString & _t1, const QString & _t2, double _t3, double _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3, _t4);
}
QT_WARNING_POP
