/****************************************************************************
** Meta object code from reading C++ file 'qpdfbookmarkmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qpdfbookmarkmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qpdfbookmarkmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QPdfBookmarkModel_t {
    QByteArrayData data[16];
    char stringdata0[223];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QPdfBookmarkModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QPdfBookmarkModel_t qt_meta_stringdata_QPdfBookmarkModel = {
    {
QT_MOC_LITERAL(0, 0, 17), // "QPdfBookmarkModel"
QT_MOC_LITERAL(1, 18, 15), // "documentChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 13), // "QPdfDocument*"
QT_MOC_LITERAL(4, 49, 8), // "document"
QT_MOC_LITERAL(5, 58, 20), // "structureModeChanged"
QT_MOC_LITERAL(6, 79, 32), // "QPdfBookmarkModel::StructureMode"
QT_MOC_LITERAL(7, 112, 13), // "structureMode"
QT_MOC_LITERAL(8, 126, 24), // "_q_documentStatusChanged"
QT_MOC_LITERAL(9, 151, 13), // "StructureMode"
QT_MOC_LITERAL(10, 165, 8), // "TreeMode"
QT_MOC_LITERAL(11, 174, 8), // "ListMode"
QT_MOC_LITERAL(12, 183, 4), // "Role"
QT_MOC_LITERAL(13, 188, 9), // "TitleRole"
QT_MOC_LITERAL(14, 198, 9), // "LevelRole"
QT_MOC_LITERAL(15, 208, 14) // "PageNumberRole"

    },
    "QPdfBookmarkModel\0documentChanged\0\0"
    "QPdfDocument*\0document\0structureModeChanged\0"
    "QPdfBookmarkModel::StructureMode\0"
    "structureMode\0_q_documentStatusChanged\0"
    "StructureMode\0TreeMode\0ListMode\0Role\0"
    "TitleRole\0LevelRole\0PageNumberRole"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QPdfBookmarkModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       2,   36, // properties
       2,   44, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       4, 0x80000000 | 3, 0x0049510b,
       7, 0x80000000 | 9, 0x0049510b,

 // properties: notify_signal_id
       0,
       1,

 // enums: name, flags, count, data
       9, 0x0,    2,   52,
      12, 0x0,    3,   56,

 // enum data: key, value
      10, uint(QPdfBookmarkModel::TreeMode),
      11, uint(QPdfBookmarkModel::ListMode),
      13, uint(QPdfBookmarkModel::TitleRole),
      14, uint(QPdfBookmarkModel::LevelRole),
      15, uint(QPdfBookmarkModel::PageNumberRole),

       0        // eod
};

void QPdfBookmarkModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QPdfBookmarkModel *_t = static_cast<QPdfBookmarkModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->documentChanged((*reinterpret_cast< QPdfDocument*(*)>(_a[1]))); break;
        case 1: _t->structureModeChanged((*reinterpret_cast< QPdfBookmarkModel::StructureMode(*)>(_a[1]))); break;
        case 2: _t->d_func()->_q_documentStatusChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QPdfBookmarkModel::*_t)(QPdfDocument * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QPdfBookmarkModel::documentChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (QPdfBookmarkModel::*_t)(QPdfBookmarkModel::StructureMode );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QPdfBookmarkModel::structureModeChanged)) {
                *result = 1;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QPdfBookmarkModel *_t = static_cast<QPdfBookmarkModel *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QPdfDocument**>(_v) = _t->document(); break;
        case 1: *reinterpret_cast< StructureMode*>(_v) = _t->structureMode(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QPdfBookmarkModel *_t = static_cast<QPdfBookmarkModel *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setDocument(*reinterpret_cast< QPdfDocument**>(_v)); break;
        case 1: _t->setStructureMode(*reinterpret_cast< StructureMode*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject QPdfBookmarkModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QPdfBookmarkModel.data,
      qt_meta_data_QPdfBookmarkModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QPdfBookmarkModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QPdfBookmarkModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QPdfBookmarkModel.stringdata0))
        return static_cast<void*>(const_cast< QPdfBookmarkModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QPdfBookmarkModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QPdfBookmarkModel::documentChanged(QPdfDocument * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QPdfBookmarkModel::structureModeChanged(QPdfBookmarkModel::StructureMode _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
