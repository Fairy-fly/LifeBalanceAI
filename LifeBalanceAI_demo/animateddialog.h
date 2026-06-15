#ifndef ANIMATEDDIALOG_H
#define ANIMATEDDIALOG_H
#include <QDialog>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTimer>

// Animation styles
enum class AnimStyle {
    SlideFromTop,    // A: slide down from top edge + fade
    ElasticPopup,    // B: scale up from center with bounce
    RiseFromBottom,  // C: slide up from bottom edge
    IrisReveal       // E: circular reveal from center
};

class AnimatedDialog : public QDialog {
    Q_OBJECT
public:
    explicit AnimatedDialog(QWidget *p = nullptr, AnimStyle style = AnimStyle::SlideFromTop);
    void setMessage(const QString &t);
    void setTitle(const QString &t);
    void setType(const QString &t);
    void showAnimated();

    static void info(QWidget *p, const QString &t, const QString &m);
    static void warn(QWidget *p, const QString &t, const QString &m);
    static void success(QWidget *p, const QString &t, const QString &m);
    static bool confirm(QWidget *p, const QString &t, const QString &m);
    // Returns: -1 = cancelled/closed, 0 = btn1 clicked, 1 = btn2 clicked
    static int choose(QWidget *p, const QString &t, const QString &m, const QString &btn1, const QString &btn2);

protected:
    void paintEvent(QPaintEvent *e) override;
    void showEvent(QShowEvent *e) override;
private:
    void setupUi();
    void refreshMessageScrollLimit();
    QLabel *m_ttl=nullptr, *m_msg=nullptr;
    QScrollArea *m_msgScroll=nullptr;
    QPushButton *m_ok=nullptr, *m_btn2=nullptr;
    QWidget *m_accentBar=nullptr;
    AnimStyle m_style;
    QString m_type = "info";
    int m_choiceResult = 0;
};

class AnimatedInputDialog : public QDialog {
    Q_OBJECT
public:
    explicit AnimatedInputDialog(QWidget *p = nullptr);
    void setTitle(const QString &t);
    void setPrompt(const QString &p);
    QString textValue() const;
    void showAnimated();
    static QString getText(QWidget *p, const QString &t, const QString &pm, const QString &def=QString(), bool *ok=nullptr);
protected:
    void paintEvent(QPaintEvent *e) override;
private:
    void setupUi();
    QLabel *m_ttl=nullptr, *m_pmt=nullptr;
    QLineEdit *m_inp=nullptr;
    QPushButton *m_ok=nullptr, *m_ccl=nullptr;
    QString m_result;
};
#endif
