/****************************************************************************
** Meta object code from reading C++ file 'appdbusadaptor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/appdbusadaptor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'appdbusadaptor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AppDBusAdaptor_t {
    QByteArrayData data[12];
    char stringdata0[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AppDBusAdaptor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AppDBusAdaptor_t qt_meta_stringdata_AppDBusAdaptor = {
    {
QT_MOC_LITERAL(0, 0, 14), // "AppDBusAdaptor"
QT_MOC_LITERAL(1, 15, 15), // "D-Bus Interface"
QT_MOC_LITERAL(2, 31, 28), // "org.hafsoftdz.harbour_thakir"
QT_MOC_LITERAL(3, 60, 4), // "open"
QT_MOC_LITERAL(4, 65, 0), // ""
QT_MOC_LITERAL(5, 66, 15), // "onActionInvoked"
QT_MOC_LITERAL(6, 82, 2), // "id"
QT_MOC_LITERAL(7, 85, 6), // "action"
QT_MOC_LITERAL(8, 92, 8), // "activate"
QT_MOC_LITERAL(9, 101, 4), // "show"
QT_MOC_LITERAL(10, 106, 8), // "Activate"
QT_MOC_LITERAL(11, 115, 13) // "platform_data"

    },
    "AppDBusAdaptor\0D-Bus Interface\0"
    "org.hafsoftdz.harbour_thakir\0open\0\0"
    "onActionInvoked\0id\0action\0activate\0"
    "show\0Activate\0platform_data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AppDBusAdaptor[] = {

 // content:
       7,       // revision
       0,       // classname
       1,   14, // classinfo
       5,   16, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // classinfo: key, value
       1,    2,

 // slots: name, argc, parameters, tag, flags
       3,    0,   41,    4, 0x0a /* Public */,
       5,    2,   42,    4, 0x0a /* Public */,
       8,    0,   47,    4, 0x0a /* Public */,
       9,    0,   48,    4, 0x0a /* Public */,
      10,    1,   49,    4, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,    6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,   11,

       0        // eod
};

void AppDBusAdaptor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AppDBusAdaptor *_t = static_cast<AppDBusAdaptor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->open(); break;
        case 1: _t->onActionInvoked((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->activate(); break;
        case 3: _t->show(); break;
        case 4: _t->Activate((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject AppDBusAdaptor::staticMetaObject = {
    { &QDBusAbstractAdaptor::staticMetaObject, qt_meta_stringdata_AppDBusAdaptor.data,
      qt_meta_data_AppDBusAdaptor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AppDBusAdaptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppDBusAdaptor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AppDBusAdaptor.stringdata0))
        return static_cast<void*>(const_cast< AppDBusAdaptor*>(this));
    return QDBusAbstractAdaptor::qt_metacast(_clname);
}

int AppDBusAdaptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractAdaptor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
struct qt_meta_stringdata_FreedesktopAppAdaptor_t {
    QByteArrayData data[11];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FreedesktopAppAdaptor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FreedesktopAppAdaptor_t qt_meta_stringdata_FreedesktopAppAdaptor = {
    {
QT_MOC_LITERAL(0, 0, 21), // "FreedesktopAppAdaptor"
QT_MOC_LITERAL(1, 22, 15), // "D-Bus Interface"
QT_MOC_LITERAL(2, 38, 27), // "org.freedesktop.Application"
QT_MOC_LITERAL(3, 66, 8), // "Activate"
QT_MOC_LITERAL(4, 75, 0), // ""
QT_MOC_LITERAL(5, 76, 13), // "platform_data"
QT_MOC_LITERAL(6, 90, 4), // "Open"
QT_MOC_LITERAL(7, 95, 4), // "uris"
QT_MOC_LITERAL(8, 100, 14), // "ActivateAction"
QT_MOC_LITERAL(9, 115, 11), // "action_name"
QT_MOC_LITERAL(10, 127, 9) // "parameter"

    },
    "FreedesktopAppAdaptor\0D-Bus Interface\0"
    "org.freedesktop.Application\0Activate\0"
    "\0platform_data\0Open\0uris\0ActivateAction\0"
    "action_name\0parameter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FreedesktopAppAdaptor[] = {

 // content:
       7,       // revision
       0,       // classname
       1,   14, // classinfo
       3,   16, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // classinfo: key, value
       1,    2,

 // slots: name, argc, parameters, tag, flags
       3,    1,   31,    4, 0x0a /* Public */,
       6,    2,   34,    4, 0x0a /* Public */,
       8,    3,   39,    4, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QVariantMap,    5,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QVariantMap,    7,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantList, QMetaType::QVariantMap,    9,   10,    5,

       0        // eod
};

void FreedesktopAppAdaptor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FreedesktopAppAdaptor *_t = static_cast<FreedesktopAppAdaptor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Activate((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 1: _t->Open((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2]))); break;
        case 2: _t->ActivateAction((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariantList(*)>(_a[2])),(*reinterpret_cast< const QVariantMap(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject FreedesktopAppAdaptor::staticMetaObject = {
    { &QDBusAbstractAdaptor::staticMetaObject, qt_meta_stringdata_FreedesktopAppAdaptor.data,
      qt_meta_data_FreedesktopAppAdaptor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FreedesktopAppAdaptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FreedesktopAppAdaptor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FreedesktopAppAdaptor.stringdata0))
        return static_cast<void*>(const_cast< FreedesktopAppAdaptor*>(this));
    return QDBusAbstractAdaptor::qt_metacast(_clname);
}

int FreedesktopAppAdaptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractAdaptor::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
