/****************************************************************************
** Meta object code from reading C++ file 'athannotification.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/athannotification.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'athannotification.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AthanNotification_t {
    QByteArrayData data[8];
    char stringdata0[90];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AthanNotification_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AthanNotification_t qt_meta_stringdata_AthanNotification = {
    {
QT_MOC_LITERAL(0, 0, 17), // "AthanNotification"
QT_MOC_LITERAL(1, 18, 13), // "stopRequested"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 15), // "onActionInvoked"
QT_MOC_LITERAL(4, 49, 2), // "id"
QT_MOC_LITERAL(5, 52, 9), // "actionKey"
QT_MOC_LITERAL(6, 62, 20), // "onNotificationClosed"
QT_MOC_LITERAL(7, 83, 6) // "reason"

    },
    "AthanNotification\0stopRequested\0\0"
    "onActionInvoked\0id\0actionKey\0"
    "onNotificationClosed\0reason"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AthanNotification[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    2,   30,    2, 0x0a /* Public */,
       6,    2,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,    4,    5,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,    4,    7,

       0        // eod
};

void AthanNotification::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AthanNotification *_t = static_cast<AthanNotification *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stopRequested(); break;
        case 1: _t->onActionInvoked((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->onNotificationClosed((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AthanNotification::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AthanNotification::stopRequested)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject AthanNotification::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AthanNotification.data,
      qt_meta_data_AthanNotification,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AthanNotification::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AthanNotification::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AthanNotification.stringdata0))
        return static_cast<void*>(const_cast< AthanNotification*>(this));
    return QObject::qt_metacast(_clname);
}

int AthanNotification::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void AthanNotification::stopRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
