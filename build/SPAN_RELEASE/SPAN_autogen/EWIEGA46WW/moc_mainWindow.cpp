/****************************************************************************
** Meta object code from reading C++ file 'mainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../mainWindow.h"
#include <QtGui/qscreen.h>
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10mainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto mainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10mainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "mainWindow",
        "addSpanButtonClicked",
        "",
        "removeSelectedSpanFiles",
        "updateRemoveSpanFilesButtonState",
        "QItemSelection",
        "selected",
        "deselected",
        "viewSpanFile",
        "onSpanFileDoubleClicked",
        "QModelIndex",
        "index",
        "onConfiguredChannelSelected",
        "onRadioButtonClicked",
        "QAbstractButton*",
        "onChannelsComboBoxIndexChanged",
        "moveUpButtonClicked",
        "moveDownButtonClicked",
        "removeConfigButtonClicked",
        "updateConfigButtonClicked",
        "addConfigButtonClicked",
        "configureButtonClicked",
        "applyConfigToAllButtonClicked",
        "deriveButtonClicked",
        "clearAllDerivedButtonClicked",
        "getCSVFileButtonClicked",
        "clearAllLandmarks",
        "removeSelectedLandmark",
        "onLandmarkSelectionChanged",
        "onLandmarkItemChanged",
        "QStandardItem*",
        "item",
        "updateRemoveLandmarkButtonState",
        "onLabelClicked",
        "channelName",
        "offset",
        "onLabelMoved",
        "labelName",
        "newX",
        "oldX",
        "onLandmarkAddedFromLabel",
        "channel",
        "lblName",
        "placeLabelsButtonClicked",
        "zoomInButtonClicked",
        "zoomOutButtonClicked",
        "resetButtonClicked",
        "on_selectButton_clicked",
        "onRangeScrollBarValueChanged",
        "value",
        "updateTrackedParameter",
        "playSoundButtonClicked",
        "playSelectionButtonClicked",
        "stopPlayback",
        "togglePlay",
        "pushButtonClicked",
        "invertButtonClicked",
        "rebuildSpectrogramForVisibleRange",
        "showFileInfoDialog",
        "on_saveTemplateButton_clicked",
        "on_applyTemplateButton_clicked",
        "on_batchApplyTemplateButton_clicked",
        "resetToDefaults",
        "syncAudioWidthToKinematicViewport",
        "importTemplateFromFile",
        "onSpecApplyButtonClicked",
        "onSpecDefaultsButtonClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'addSpanButtonClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'removeSelectedSpanFiles'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateRemoveSpanFilesButtonState'
        QtMocHelpers::SlotData<void(const QItemSelection &, const QItemSelection &)>(4, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 5, 6 }, { 0x80000000 | 5, 7 },
        }}),
        // Slot 'viewSpanFile'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSpanFileDoubleClicked'
        QtMocHelpers::SlotData<void(const QModelIndex &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'onConfiguredChannelSelected'
        QtMocHelpers::SlotData<void(const QItemSelection &, const QItemSelection &)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 5, 6 }, { 0x80000000 | 5, 7 },
        }}),
        // Slot 'onRadioButtonClicked'
        QtMocHelpers::SlotData<void(QAbstractButton *)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 14, 2 },
        }}),
        // Slot 'onChannelsComboBoxIndexChanged'
        QtMocHelpers::SlotData<void(int)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 11 },
        }}),
        // Slot 'moveUpButtonClicked'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'moveDownButtonClicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'removeConfigButtonClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateConfigButtonClicked'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'addConfigButtonClicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'configureButtonClicked'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'applyConfigToAllButtonClicked'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'deriveButtonClicked'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'clearAllDerivedButtonClicked'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'getCSVFileButtonClicked'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'clearAllLandmarks'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'removeSelectedLandmark'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLandmarkSelectionChanged'
        QtMocHelpers::SlotData<void(const QItemSelection &, const QItemSelection &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 5, 6 }, { 0x80000000 | 5, 7 },
        }}),
        // Slot 'onLandmarkItemChanged'
        QtMocHelpers::SlotData<void(QStandardItem *)>(29, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 30, 31 },
        }}),
        // Slot 'updateRemoveLandmarkButtonState'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLabelClicked'
        QtMocHelpers::SlotData<void(const QString &, double)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 34 }, { QMetaType::Double, 35 },
        }}),
        // Slot 'onLabelMoved'
        QtMocHelpers::SlotData<void(const QString &, const QString &, double, double)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 34 }, { QMetaType::QString, 37 }, { QMetaType::Double, 38 }, { QMetaType::Double, 39 },
        }}),
        // Slot 'onLandmarkAddedFromLabel'
        QtMocHelpers::SlotData<void(const QString &, double, const QString &)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 41 }, { QMetaType::Double, 35 }, { QMetaType::QString, 42 },
        }}),
        // Slot 'placeLabelsButtonClicked'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'zoomInButtonClicked'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'zoomOutButtonClicked'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'resetButtonClicked'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_selectButton_clicked'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRangeScrollBarValueChanged'
        QtMocHelpers::SlotData<void(int)>(48, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 49 },
        }}),
        // Slot 'updateTrackedParameter'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'playSoundButtonClicked'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'playSelectionButtonClicked'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'stopPlayback'
        QtMocHelpers::SlotData<void()>(53, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'togglePlay'
        QtMocHelpers::SlotData<void()>(54, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'pushButtonClicked'
        QtMocHelpers::SlotData<void()>(55, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'invertButtonClicked'
        QtMocHelpers::SlotData<void()>(56, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'rebuildSpectrogramForVisibleRange'
        QtMocHelpers::SlotData<void()>(57, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showFileInfoDialog'
        QtMocHelpers::SlotData<void()>(58, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_saveTemplateButton_clicked'
        QtMocHelpers::SlotData<void()>(59, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_applyTemplateButton_clicked'
        QtMocHelpers::SlotData<void()>(60, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_batchApplyTemplateButton_clicked'
        QtMocHelpers::SlotData<void()>(61, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'resetToDefaults'
        QtMocHelpers::SlotData<void()>(62, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'syncAudioWidthToKinematicViewport'
        QtMocHelpers::SlotData<void()>(63, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'importTemplateFromFile'
        QtMocHelpers::SlotData<void()>(64, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSpecApplyButtonClicked'
        QtMocHelpers::SlotData<void()>(65, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSpecDefaultsButtonClicked'
        QtMocHelpers::SlotData<void()>(66, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<mainWindow, qt_meta_tag_ZN10mainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject mainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10mainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10mainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10mainWindowE_t>.metaTypes,
    nullptr
} };

void mainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<mainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addSpanButtonClicked(); break;
        case 1: _t->removeSelectedSpanFiles(); break;
        case 2: _t->updateRemoveSpanFilesButtonState((*reinterpret_cast<std::add_pointer_t<QItemSelection>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QItemSelection>>(_a[2]))); break;
        case 3: _t->viewSpanFile(); break;
        case 4: _t->onSpanFileDoubleClicked((*reinterpret_cast<std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 5: _t->onConfiguredChannelSelected((*reinterpret_cast<std::add_pointer_t<QItemSelection>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QItemSelection>>(_a[2]))); break;
        case 6: _t->onRadioButtonClicked((*reinterpret_cast<std::add_pointer_t<QAbstractButton*>>(_a[1]))); break;
        case 7: _t->onChannelsComboBoxIndexChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->moveUpButtonClicked(); break;
        case 9: _t->moveDownButtonClicked(); break;
        case 10: _t->removeConfigButtonClicked(); break;
        case 11: _t->updateConfigButtonClicked(); break;
        case 12: _t->addConfigButtonClicked(); break;
        case 13: _t->configureButtonClicked(); break;
        case 14: _t->applyConfigToAllButtonClicked(); break;
        case 15: _t->deriveButtonClicked(); break;
        case 16: _t->clearAllDerivedButtonClicked(); break;
        case 17: _t->getCSVFileButtonClicked(); break;
        case 18: _t->clearAllLandmarks(); break;
        case 19: _t->removeSelectedLandmark(); break;
        case 20: _t->onLandmarkSelectionChanged((*reinterpret_cast<std::add_pointer_t<QItemSelection>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QItemSelection>>(_a[2]))); break;
        case 21: _t->onLandmarkItemChanged((*reinterpret_cast<std::add_pointer_t<QStandardItem*>>(_a[1]))); break;
        case 22: _t->updateRemoveLandmarkButtonState(); break;
        case 23: _t->onLabelClicked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 24: _t->onLabelMoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[4]))); break;
        case 25: _t->onLandmarkAddedFromLabel((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 26: _t->placeLabelsButtonClicked(); break;
        case 27: _t->zoomInButtonClicked(); break;
        case 28: _t->zoomOutButtonClicked(); break;
        case 29: _t->resetButtonClicked(); break;
        case 30: _t->on_selectButton_clicked(); break;
        case 31: _t->onRangeScrollBarValueChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 32: _t->updateTrackedParameter(); break;
        case 33: _t->playSoundButtonClicked(); break;
        case 34: _t->playSelectionButtonClicked(); break;
        case 35: _t->stopPlayback(); break;
        case 36: _t->togglePlay(); break;
        case 37: _t->pushButtonClicked(); break;
        case 38: _t->invertButtonClicked(); break;
        case 39: _t->rebuildSpectrogramForVisibleRange(); break;
        case 40: _t->showFileInfoDialog(); break;
        case 41: _t->on_saveTemplateButton_clicked(); break;
        case 42: _t->on_applyTemplateButton_clicked(); break;
        case 43: _t->on_batchApplyTemplateButton_clicked(); break;
        case 44: _t->resetToDefaults(); break;
        case 45: _t->syncAudioWidthToKinematicViewport(); break;
        case 46: _t->importTemplateFromFile(); break;
        case 47: _t->onSpecApplyButtonClicked(); break;
        case 48: _t->onSpecDefaultsButtonClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QItemSelection >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QItemSelection >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractButton* >(); break;
            }
            break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QItemSelection >(); break;
            }
            break;
        }
    }
}

const QMetaObject *mainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10mainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int mainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 49)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 49;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 49)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 49;
    }
    return _id;
}
QT_WARNING_POP
