#include "LogManager.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QDockWidget>

LogManager::LogManager(QMainWindow *mainWindow, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow)
{
    m_logOutput = new QTextEdit(m_mainWindow);
    m_logOutput->setReadOnly(true);

    m_logDock = new QDockWidget(tr("Log"), m_mainWindow);
    m_logDock->setWidget(m_logOutput);
    m_logDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
}

void LogManager::logMessage(const QString &message)
{
    m_logOutput->append(message);
}