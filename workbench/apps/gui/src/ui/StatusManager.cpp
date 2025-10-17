#include "StatusManager.h"
#include <QMainWindow>
#include <QStatusBar>
#include <QLabel>

StatusManager::StatusManager(QMainWindow *mainWindow, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow)
{
    m_statusLabel = new QLabel(m_mainWindow);
    m_mainWindow->statusBar()->addWidget(m_statusLabel);
    m_statusLabel->setText("Ready");
}

void StatusManager::showMessage(const QString &message)
{
    m_statusLabel->setText(message);
}

void StatusManager::updateForMode(ApplicationMode mode)
{
    if (mode == ApplicationMode::Edit)
    {
        m_statusLabel->setText(tr("Edit Mode - Authoring tools available"));
    }
    else
    {
        m_statusLabel->setText(tr("View Mode - Viewing tools available"));
    }
}