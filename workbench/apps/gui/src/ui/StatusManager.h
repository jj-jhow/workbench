#pragma once
#include <QObject>
#include <QString>
#include "core/ApplicationMode.h"

QT_BEGIN_NAMESPACE
class QMainWindow;
class QLabel;
QT_END_NAMESPACE

class StatusManager : public QObject
{
    Q_OBJECT

public:
    explicit StatusManager(QMainWindow *mainWindow, QObject *parent = nullptr);

public slots:
    void showMessage(const QString &message);
    void updateForMode(ApplicationMode mode);

private:
    QMainWindow *m_mainWindow;
    QLabel *m_statusLabel;
};