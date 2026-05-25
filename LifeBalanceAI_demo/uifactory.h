#ifndef UIFACTORY_H
#define UIFACTORY_H

#include <QString>

class QLabel;
class QPixmap;
class QPushButton;
class QLineEdit;
class QWidget;

namespace UiFactory {

QPixmap loadPixmap(const QString &resourcePath);
QLabel *assetLabel(const QString &resourcePath, int minHeight, QWidget *parent = nullptr);
QWidget *emptyState(const QString &assetPath, const QString &title, const QString &body, QWidget *parent = nullptr);
QPushButton *primaryButton(const QString &text, QWidget *parent = nullptr);
QPushButton *secondaryButton(const QString &text, QWidget *parent = nullptr);
QPushButton *ghostButton(const QString &text, QWidget *parent = nullptr);
QPushButton *dangerButton(const QString &text, QWidget *parent = nullptr);
QPushButton *smallButton(const QString &text, QWidget *parent = nullptr);
QLineEdit *textInput(const QString &placeholder, QWidget *parent = nullptr);
QWidget *infoCard(const QString &title, const QString &body, QWidget *parent = nullptr);
QWidget *healthCard(const QString &title, const QString &body, const QString &accentColor, QWidget *parent = nullptr);
void applyWarmPolish(QWidget *root);
void setClass(QWidget *widget, const QString &className);

} // namespace UiFactory

#endif // UIFACTORY_H
