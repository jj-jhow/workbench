#include "MenuManager.h"
#include "actions/FileActions.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

MenuManager::MenuManager(QMainWindow *mainWindow, FileActions *fileActions, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow), m_fileActions(fileActions)
{
    createFileMenu();
    createModeMenu();
    createHelpMenu();
}

void MenuManager::createFileMenu()
{
    m_fileMenu = m_mainWindow->menuBar()->addMenu(tr("&File"));

    m_openAct = new QAction(tr("Open USD File..."), this);
    connect(m_openAct, &QAction::triggered, m_fileActions, &FileActions::openUsdFile);
    m_fileMenu->addAction(m_openAct);

    m_convertAct = new QAction(tr("Convert File..."), this);
    connect(m_convertAct, &QAction::triggered, m_fileActions, &FileActions::convertFile);
    m_fileMenu->addAction(m_convertAct);

    m_closeStageAct = new QAction(tr("Close Stage"), this);
    connect(m_closeStageAct, &QAction::triggered, m_fileActions, &FileActions::closeStage);
    m_fileMenu->addAction(m_closeStageAct);

    m_fileMenu->addSeparator();

    m_exitAct = new QAction(tr("E&xit"), this);
    connect(m_exitAct, &QAction::triggered, this, &MenuManager::quitRequested);
    m_fileMenu->addAction(m_exitAct);
}

void MenuManager::createModeMenu()
{
    m_modeMenu = m_mainWindow->menuBar()->addMenu(tr("&Mode"));

    m_modeToggleAct = new QAction(tr("Switch to Edit Mode"), this);
    connect(m_modeToggleAct, &QAction::triggered, this, &MenuManager::modeToggleRequested);
    m_modeMenu->addAction(m_modeToggleAct);
}

void MenuManager::createHelpMenu()
{
    m_helpMenu = m_mainWindow->menuBar()->addMenu(tr("&Help"));

    m_helpAct = new QAction(tr("Help"), this);
    connect(m_helpAct, &QAction::triggered, this, &MenuManager::helpRequested);
    m_helpMenu->addAction(m_helpAct);

    m_aboutAct = new QAction(tr("About"), this);
    connect(m_aboutAct, &QAction::triggered, this, &MenuManager::aboutRequested);
    m_helpMenu->addAction(m_aboutAct);
}

void MenuManager::updateForMode(ApplicationMode mode)
{
    if (mode == ApplicationMode::Edit)
    {
        m_modeToggleAct->setText(tr("Switch to View Mode"));
    }
    else
    {
        m_modeToggleAct->setText(tr("Switch to Edit Mode"));
    }
}