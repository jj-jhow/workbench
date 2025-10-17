#pragma once
#include <QObject>
#include <QString>

QT_BEGIN_NAMESPACE
class QMainWindow;
class QTextEdit;
class QDockWidget;
QT_END_NAMESPACE

class LogManager : public QObject
{
    Q_OBJECT

public:
    explicit LogManager(QMainWindow *mainWindow, QObject *parent = nullptr);

public slots:
    void logMessage(const QString &message);

private:
    QMainWindow *m_mainWindow;
    QTextEdit *m_logOutput;
    QDockWidget *m_logDock;
};