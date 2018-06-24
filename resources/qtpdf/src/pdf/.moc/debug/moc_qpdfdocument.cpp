/****************************************************************************
** Meta object code from reading C++ file 'qpdfdocument.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qpdfdocument.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qpdfdocument.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QPdfDocument_t {
    QByteArrayData data[34];
    char stringdata0[461];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QPdfDocument_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QPdfDocument_t qt_meta_stringdata_QPdfDocument = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QPdfDocument"
QT_MOC_LITERAL(1, 13, 15), // "passwordChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 16), // "passwordRequired"
QT_MOC_LITERAL(4, 47, 13), // "statusChanged"
QT_MOC_LITERAL(5, 61, 20), // "QPdfDocument::Status"
QT_MOC_LITERAL(6, 82, 6), // "status"
QT_MOC_LITERAL(7, 89, 16), // "pageCountChanged"
QT_MOC_LITERAL(8, 106, 9), // "pageCount"
QT_MOC_LITERAL(9, 116, 38), // "_q_tryLoadingWithSizeFromCont..."
QT_MOC_LITERAL(10, 155, 33), // "_q_copyFromSequentialSourceDe..."
QT_MOC_LITERAL(11, 189, 8), // "password"
QT_MOC_LITERAL(12, 198, 6), // "Status"
QT_MOC_LITERAL(13, 205, 4), // "Null"
QT_MOC_LITERAL(14, 210, 7), // "Loading"
QT_MOC_LITERAL(15, 218, 5), // "Ready"
QT_MOC_LITERAL(16, 224, 9), // "Unloading"
QT_MOC_LITERAL(17, 234, 5), // "Error"
QT_MOC_LITERAL(18, 240, 13), // "DocumentError"
QT_MOC_LITERAL(19, 254, 7), // "NoError"
QT_MOC_LITERAL(20, 262, 12), // "UnknownError"
QT_MOC_LITERAL(21, 275, 17), // "FileNotFoundError"
QT_MOC_LITERAL(22, 293, 22), // "InvalidFileFormatError"
QT_MOC_LITERAL(23, 316, 22), // "IncorrectPasswordError"
QT_MOC_LITERAL(24, 339, 30), // "UnsupportedSecuritySchemeError"
QT_MOC_LITERAL(25, 370, 13), // "MetaDataField"
QT_MOC_LITERAL(26, 384, 5), // "Title"
QT_MOC_LITERAL(27, 390, 7), // "Subject"
QT_MOC_LITERAL(28, 398, 6), // "Author"
QT_MOC_LITERAL(29, 405, 8), // "Keywords"
QT_MOC_LITERAL(30, 414, 8), // "Producer"
QT_MOC_LITERAL(31, 423, 7), // "Creator"
QT_MOC_LITERAL(32, 431, 12), // "CreationDate"
QT_MOC_LITERAL(33, 444, 16) // "ModificationDate"

    },
    "QPdfDocument\0passwordChanged\0\0"
    "passwordRequired\0statusChanged\0"
    "QPdfDocument::Status\0status\0"
    "pageCountChanged\0pageCount\0"
    "_q_tryLoadingWithSizeFromContentHeader\0"
    "_q_copyFromSequentialSourceDevice\0"
    "password\0Status\0Null\0Loading\0Ready\0"
    "Unloading\0Error\0DocumentError\0NoError\0"
    "UnknownError\0FileNotFoundError\0"
    "InvalidFileFormatError\0IncorrectPasswordError\0"
    "UnsupportedSecuritySchemeError\0"
    "MetaDataField\0Title\0Subject\0Author\0"
    "Keywords\0Producer\0Creator\0CreationDate\0"
    "ModificationDate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QPdfDocument[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       3,   54, // properties
       3,   66, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,
       4,    1,   46,    2, 0x06 /* Public */,
       7,    1,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   52,    2, 0x08 /* Private */,
      10,    0,   53,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Int,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       8, QMetaType::Int, 0x00495801,
      11, QMetaType::QString, 0x00495903,
       6, 0x80000000 | 12, 0x00495809,

 // properties: notify_signal_id
       3,
       0,
       2,

 // enums: name, flags, count, data
      12, 0x0,    5,   78,
      18, 0x0,    6,   88,
      25, 0x0,    8,  100,

 // enum data: key, value
      13, uint(QPdfDocument::Null),
      14, uint(QPdfDocument::Loading),
      15, uint(QPdfDocument::Ready),
      16, uint(QPdfDocument::Unloading),
      17, uint(QPdfDocument::Error),
      19, uint(QPdfDocument::NoError),
      20, uint(QPdfDocument::UnknownError),
      21, uint(QPdfDocument::FileNotFoundError),
      22, uint(QPdfDocument::InvalidFileFormatError),
      23, uint(QPdfDocument::IncorrectPasswordError),
      24, uint(QPdfDocument::UnsupportedSecuritySchemeError),
      26, uint(QPdfDocument::Title),
      27, uint(QPdfDocument::Subject),
      28, uint(QPdfDocument::Author),
      29, uint(QPdfDocument::Keywords),
      30, uint(QPdfDocument::Producer),
      31, uint(QPdfDocument::Creator),
      32, uint(QPdfDocument::CreationDate),
      33, uint(QPdfDocument::ModificationDate),

       0        // eod
};

void QPdfDocument::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QPdfDocument *_t = static_cast<QPdfDocument *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->passwordChanged(); break;
        case 1: _t->passwordRequired(); break;
        case 2: _t->statusChanged((*reinterpret_cast< QPdfDocument::Status(*)>(_a[1]))); break;
        case 3: _t->pageCountChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->d->_q_tryLoadingWithSizeFromContentHeader(); break;
        case 5: _t->d->_q_copyFromSequentialSourceDevice(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QPdfDocument::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QPdfDocument::passwordChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (QPdfDocument::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QPdfDocument::passwordRequired)) {
                *result = 1;
            }
        }
        {
            typedef void (QPdfDocument::*_t)(QPdfDocument::Status );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QPdfDocument::statusChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (QPdfDocument::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QPdfDocument::pageCountChanged)) {
                *result = 3;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QPdfDocument *_t = static_cast<QPdfDocument *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->pageCount(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->password(); break;
        case 2: *reinterpret_cast< Status*>(_v) = _t->status(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QPdfDocument *_t = static_cast<QPdfDocument *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setPassword(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject QPdfDocument::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QPdfDocument.data,
      qt_meta_data_QPdfDocument,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QPdfDocument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QPdfDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QPdfDocument.stringdata0))
        return static_cast<void*>(const_cast< QPdfDocument*>(this));
    return QObject::qt_metacast(_clname);
}

int QPdfDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QPdfDocument::passwordChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QPdfDocument::passwordRequired()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void QPdfDocument::statusChanged(QPdfDocument::Status _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QPdfDocument::pageCountChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
