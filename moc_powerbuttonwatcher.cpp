/****************************************************************************
** Meta object code from reading C++ file 'powerbuttonwatcher.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/powerbuttonwatcher.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'powerbuttonwatcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PowerButtonWatcher_t {
    QByteArrayData data[16];
    char stringdata0[236];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PowerButtonWatcher_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PowerButtonWatcher_t qt_meta_stringdata_PowerButtonWatcher = {
    {
QT_MOC_LITERAL(0, 0, 18), // "PowerButtonWatcher"
QT_MOC_LITERAL(1, 19, 13), // "stopTriggered"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 18), // "powerButtonPressed"
QT_MOC_LITERAL(4, 53, 13), // "onPactlOutput"
QT_MOC_LITERAL(5, 67, 16), // "onProfileChanged"
QT_MOC_LITERAL(6, 84, 20), // "onPowerButtonTrigger"
QT_MOC_LITERAL(7, 105, 5), // "event"
QT_MOC_LITERAL(8, 111, 20), // "onAlarmUiFeedbackInd"
QT_MOC_LITERAL(9, 132, 25), // "onVolumePropertiesChanged"
QT_MOC_LITERAL(10, 158, 9), // "interface"
QT_MOC_LITERAL(11, 168, 12), // "changedProps"
QT_MOC_LITERAL(12, 181, 11), // "invalidated"
QT_MOC_LITERAL(13, 193, 17), // "checkVolumeChange"
QT_MOC_LITERAL(14, 211, 18), // "onDisplayStatusInd"
QT_MOC_LITERAL(15, 230, 5) // "state"

    },
    "PowerButtonWatcher\0stopTriggered\0\0"
    "powerButtonPressed\0onPactlOutput\0"
    "onProfileChanged\0onPowerButtonTrigger\0"
    "event\0onAlarmUiFeedbackInd\0"
    "onVolumePropertiesChanged\0interface\0"
    "changedProps\0invalidated\0checkVolumeChange\0"
    "onDisplayStatusInd\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PowerButtonWatcher[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    1,   63,    2, 0x0a /* Public */,
       8,    1,   66,    2, 0x0a /* Public */,
       9,    3,   69,    2, 0x0a /* Public */,
      13,    0,   76,    2, 0x0a /* Public */,
      14,    1,   77,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantMap, QMetaType::QStringList,   10,   11,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

void PowerButtonWatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PowerButtonWatcher *_t = static_cast<PowerButtonWatcher *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stopTriggered(); break;
        case 1: _t->powerButtonPressed(); break;
        case 2: _t->onPactlOutput(); break;
        case 3: _t->onProfileChanged(); break;
        case 4: _t->onPowerButtonTrigger((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onAlarmUiFeedbackInd((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onVolumePropertiesChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3]))); break;
        case 7: _t->checkVolumeChange(); break;
        case 8: _t->onDisplayStatusInd((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PowerButtonWatcher::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PowerButtonWatcher::stopTriggered)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (PowerButtonWatcher::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PowerButtonWatcher::powerButtonPressed)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject PowerButtonWatcher::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PowerButtonWatcher.data,
      qt_meta_data_PowerButtonWatcher,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PowerButtonWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PowerButtonWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PowerButtonWatcher.stringdata0))
        return static_cast<void*>(const_cast< PowerButtonWatcher*>(this));
    if (!strcmp(_clname, "QOrientationFilter"))
        return static_cast< QOrientationFilter*>(const_cast< PowerButtonWatcher*>(this));
    return QObject::qt_metacast(_clname);
}

int PowerButtonWatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void PowerButtonWatcher::stopTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void PowerButtonWatcher::powerButtonPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
