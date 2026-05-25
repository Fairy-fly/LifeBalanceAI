#ifndef FEEDBACKDIALOG_H
#define FEEDBACKDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>

class FeedbackDialog : public QDialog
{
    Q_OBJECT

public:
    /** Normal constructor — empty text */
    explicit FeedbackDialog(const QString &slotTitle, QWidget *parent = nullptr);

    /** Constructor with pre-filled text (for "修改反馈") */
    explicit FeedbackDialog(const QString &slotTitle,
                            const QString &initialText,
                            QWidget *parent = nullptr);

    QString feedbackText() const;
    QString imagePath() const;

private slots:
    void onSelectImage();

private:
    void setupUi(const QString &slotTitle, const QString &initialText = QString());

    QTextEdit   *m_textEdit;
    QPushButton *m_btnConfirm;
    QPushButton *m_btnCancel;
    QPushButton *m_btnSelectImage;
    QLabel      *m_lblImagePreview;
    QString      m_imagePath;
};

#endif // FEEDBACKDIALOG_H
