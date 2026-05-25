/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "on_btnGoRegister_clicked",
    "",
    "on_btnGoLogin_clicked",
    "on_btnRegister_clicked",
    "on_btnSendVerifyCode_clicked",
    "on_btnLogin_clicked",
    "on_btnSaveProfile_clicked",
    "onPlanGenerated",
    "jsonResult",
    "requestAIPlan",
    "onFeedbackButtonClicked",
    "QTextEdit*",
    "textEdit",
    "slotTitle",
    "onPartialUpdateGenerated",
    "onViewYesterdayClicked",
    "onAdjustPlanClicked",
    "slotKey",
    "onTabChanged",
    "index",
    "on_btnSaveProfile5_clicked",
    "on_btnBackToMain_clicked",
    "on_btnPay6_clicked",
    "on_btnBack6_clicked",
    "onDeepAnalysisTriggered",
    "onDeepAnalysisReady",
    "userId",
    "LifeBalanceAI::Models::DeepAnalysisResult",
    "result",
    "onDeepAnalysisError",
    "error",
    "onGenerateReport",
    "onExportReport",
    "reportId",
    "format",
    "onReportReady",
    "LifeBalanceAI::Models::ReportData",
    "report",
    "onReportError"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[80];
    char stringdata0[11];
    char stringdata1[25];
    char stringdata2[1];
    char stringdata3[22];
    char stringdata4[23];
    char stringdata5[29];
    char stringdata6[20];
    char stringdata7[26];
    char stringdata8[16];
    char stringdata9[11];
    char stringdata10[14];
    char stringdata11[24];
    char stringdata12[11];
    char stringdata13[9];
    char stringdata14[10];
    char stringdata15[25];
    char stringdata16[23];
    char stringdata17[20];
    char stringdata18[8];
    char stringdata19[13];
    char stringdata20[6];
    char stringdata21[27];
    char stringdata22[25];
    char stringdata23[19];
    char stringdata24[20];
    char stringdata25[24];
    char stringdata26[20];
    char stringdata27[7];
    char stringdata28[42];
    char stringdata29[7];
    char stringdata30[20];
    char stringdata31[6];
    char stringdata32[17];
    char stringdata33[15];
    char stringdata34[9];
    char stringdata35[7];
    char stringdata36[14];
    char stringdata37[34];
    char stringdata38[7];
    char stringdata39[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 24),  // "on_btnGoRegister_clicked"
        QT_MOC_LITERAL(36, 0),  // ""
        QT_MOC_LITERAL(37, 21),  // "on_btnGoLogin_clicked"
        QT_MOC_LITERAL(59, 22),  // "on_btnRegister_clicked"
        QT_MOC_LITERAL(82, 28),  // "on_btnSendVerifyCode_clicked"
        QT_MOC_LITERAL(111, 19),  // "on_btnLogin_clicked"
        QT_MOC_LITERAL(131, 25),  // "on_btnSaveProfile_clicked"
        QT_MOC_LITERAL(157, 15),  // "onPlanGenerated"
        QT_MOC_LITERAL(173, 10),  // "jsonResult"
        QT_MOC_LITERAL(184, 13),  // "requestAIPlan"
        QT_MOC_LITERAL(198, 23),  // "onFeedbackButtonClicked"
        QT_MOC_LITERAL(222, 10),  // "QTextEdit*"
        QT_MOC_LITERAL(233, 8),  // "textEdit"
        QT_MOC_LITERAL(242, 9),  // "slotTitle"
        QT_MOC_LITERAL(252, 24),  // "onPartialUpdateGenerated"
        QT_MOC_LITERAL(277, 22),  // "onViewYesterdayClicked"
        QT_MOC_LITERAL(300, 19),  // "onAdjustPlanClicked"
        QT_MOC_LITERAL(320, 7),  // "slotKey"
        QT_MOC_LITERAL(328, 12),  // "onTabChanged"
        QT_MOC_LITERAL(341, 5),  // "index"
        QT_MOC_LITERAL(347, 26),  // "on_btnSaveProfile5_clicked"
        QT_MOC_LITERAL(374, 24),  // "on_btnBackToMain_clicked"
        QT_MOC_LITERAL(399, 18),  // "on_btnPay6_clicked"
        QT_MOC_LITERAL(418, 19),  // "on_btnBack6_clicked"
        QT_MOC_LITERAL(438, 23),  // "onDeepAnalysisTriggered"
        QT_MOC_LITERAL(462, 19),  // "onDeepAnalysisReady"
        QT_MOC_LITERAL(482, 6),  // "userId"
        QT_MOC_LITERAL(489, 41),  // "LifeBalanceAI::Models::DeepAn..."
        QT_MOC_LITERAL(531, 6),  // "result"
        QT_MOC_LITERAL(538, 19),  // "onDeepAnalysisError"
        QT_MOC_LITERAL(558, 5),  // "error"
        QT_MOC_LITERAL(564, 16),  // "onGenerateReport"
        QT_MOC_LITERAL(581, 14),  // "onExportReport"
        QT_MOC_LITERAL(596, 8),  // "reportId"
        QT_MOC_LITERAL(605, 6),  // "format"
        QT_MOC_LITERAL(612, 13),  // "onReportReady"
        QT_MOC_LITERAL(626, 33),  // "LifeBalanceAI::Models::Report..."
        QT_MOC_LITERAL(660, 6),  // "report"
        QT_MOC_LITERAL(667, 13)   // "onReportError"
    },
    "MainWindow",
    "on_btnGoRegister_clicked",
    "",
    "on_btnGoLogin_clicked",
    "on_btnRegister_clicked",
    "on_btnSendVerifyCode_clicked",
    "on_btnLogin_clicked",
    "on_btnSaveProfile_clicked",
    "onPlanGenerated",
    "jsonResult",
    "requestAIPlan",
    "onFeedbackButtonClicked",
    "QTextEdit*",
    "textEdit",
    "slotTitle",
    "onPartialUpdateGenerated",
    "onViewYesterdayClicked",
    "onAdjustPlanClicked",
    "slotKey",
    "onTabChanged",
    "index",
    "on_btnSaveProfile5_clicked",
    "on_btnBackToMain_clicked",
    "on_btnPay6_clicked",
    "on_btnBack6_clicked",
    "onDeepAnalysisTriggered",
    "onDeepAnalysisReady",
    "userId",
    "LifeBalanceAI::Models::DeepAnalysisResult",
    "result",
    "onDeepAnalysisError",
    "error",
    "onGenerateReport",
    "onExportReport",
    "reportId",
    "format",
    "onReportReady",
    "LifeBalanceAI::Models::ReportData",
    "report",
    "onReportError"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  158,    2, 0x08,    1 /* Private */,
       3,    0,  159,    2, 0x08,    2 /* Private */,
       4,    0,  160,    2, 0x08,    3 /* Private */,
       5,    0,  161,    2, 0x08,    4 /* Private */,
       6,    0,  162,    2, 0x08,    5 /* Private */,
       7,    0,  163,    2, 0x08,    6 /* Private */,
       8,    1,  164,    2, 0x08,    7 /* Private */,
      10,    0,  167,    2, 0x08,    9 /* Private */,
      11,    2,  168,    2, 0x08,   10 /* Private */,
      15,    1,  173,    2, 0x08,   13 /* Private */,
      16,    0,  176,    2, 0x08,   15 /* Private */,
      17,    3,  177,    2, 0x08,   16 /* Private */,
      19,    1,  184,    2, 0x08,   20 /* Private */,
      21,    0,  187,    2, 0x08,   22 /* Private */,
      22,    0,  188,    2, 0x08,   23 /* Private */,
      23,    0,  189,    2, 0x08,   24 /* Private */,
      24,    0,  190,    2, 0x08,   25 /* Private */,
      25,    0,  191,    2, 0x08,   26 /* Private */,
      26,    2,  192,    2, 0x08,   27 /* Private */,
      30,    2,  197,    2, 0x08,   30 /* Private */,
      32,    0,  202,    2, 0x08,   33 /* Private */,
      33,    2,  203,    2, 0x08,   34 /* Private */,
      36,    2,  208,    2, 0x08,   37 /* Private */,
      39,    2,  213,    2, 0x08,   40 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12, QMetaType::QString,   13,   14,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12, QMetaType::QString, QMetaType::QString,   13,   18,   14,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 28,   27,   29,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   27,   31,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   34,   35,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 37,   27,   38,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   27,   31,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'on_btnGoRegister_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnGoLogin_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnRegister_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSendVerifyCode_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnLogin_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSaveProfile_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPlanGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'requestAIPlan'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFeedbackButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTextEdit *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onPartialUpdateGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onViewYesterdayClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAdjustPlanClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTextEdit *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onTabChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_btnSaveProfile5_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnBackToMain_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnPay6_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnBack6_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeepAnalysisTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeepAnalysisReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const LifeBalanceAI::Models::DeepAnalysisResult &, std::false_type>,
        // method 'onDeepAnalysisError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onGenerateReport'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportReport'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onReportReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const LifeBalanceAI::Models::ReportData &, std::false_type>,
        // method 'onReportError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_btnGoRegister_clicked(); break;
        case 1: _t->on_btnGoLogin_clicked(); break;
        case 2: _t->on_btnRegister_clicked(); break;
        case 3: _t->on_btnSendVerifyCode_clicked(); break;
        case 4: _t->on_btnLogin_clicked(); break;
        case 5: _t->on_btnSaveProfile_clicked(); break;
        case 6: _t->onPlanGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->requestAIPlan(); break;
        case 8: _t->onFeedbackButtonClicked((*reinterpret_cast< std::add_pointer_t<QTextEdit*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->onPartialUpdateGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->onViewYesterdayClicked(); break;
        case 11: _t->onAdjustPlanClicked((*reinterpret_cast< std::add_pointer_t<QTextEdit*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 12: _t->onTabChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->on_btnSaveProfile5_clicked(); break;
        case 14: _t->on_btnBackToMain_clicked(); break;
        case 15: _t->on_btnPay6_clicked(); break;
        case 16: _t->on_btnBack6_clicked(); break;
        case 17: _t->onDeepAnalysisTriggered(); break;
        case 18: _t->onDeepAnalysisReady((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<LifeBalanceAI::Models::DeepAnalysisResult>>(_a[2]))); break;
        case 19: _t->onDeepAnalysisError((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 20: _t->onGenerateReport(); break;
        case 21: _t->onExportReport((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 22: _t->onReportReady((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<LifeBalanceAI::Models::ReportData>>(_a[2]))); break;
        case 23: _t->onReportError((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    return _id;
}
QT_WARNING_POP
