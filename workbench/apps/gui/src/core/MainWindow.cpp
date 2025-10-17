#include "MainWindow.h"
#include "StageManager.h"
#include "widgets/SceneViewWidget.h"
#include "widgets/SceneTreeWidget.h"
#include "widgets/PrimPropertiesWidget.h"
#include "actions/FileActions.h"
#include "ui/MenuManager.h"
#include "ui/ToolbarManager.h"
#include "ui/StatusManager.h"
#include "ui/LogManager.h"

#include <QApplication>
#include <QMessageBox>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_stageManager(std::make_unique<StageManager>()), m_currentMode(ApplicationMode::View)
{
    setWindowTitle("USD Workbench");
    resize(800, 600);

    setupUI();
    setupConnections();
    updateUIForMode();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    // Create UI managers
    m_fileActions = std::make_unique<FileActions>(m_stageManager, this);
    m_menuManager = std::make_unique<MenuManager>(this, m_fileActions.get());
    m_toolbarManager = std::make_unique<ToolbarManager>(this);
    m_statusManager = std::make_unique<StatusManager>(this);
    m_logManager = std::make_unique<LogManager>(this);

    // Create main UI layout
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_sceneViewWidget = new SceneViewWidget(m_mainSplitter);
    m_rightSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);
    m_sceneTreeWidget = new SceneTreeWidget(m_rightSplitter);
    m_primPropertiesWidget = new PrimPropertiesWidget(m_rightSplitter);

    // Configure splitters
    m_rightSplitter->setStretchFactor(0, 2); // Scene tree gets more space
    m_rightSplitter->setStretchFactor(1, 1); // Properties less
    m_mainSplitter->setStretchFactor(0, 3);  // Scene view wider
    m_mainSplitter->setStretchFactor(1, 2);

    setCentralWidget(m_mainSplitter);
}

void MainWindow::setupConnections()
{
    // Connect file actions to UI updates
    connect(m_fileActions.get(), &FileActions::messageLogged,
            m_logManager.get(), &LogManager::logMessage);
    connect(m_fileActions.get(), &FileActions::messageLogged,
            m_statusManager.get(), &StatusManager::showMessage);
    connect(m_fileActions.get(), &FileActions::stageLoaded,
            this, &MainWindow::onStageLoaded);
    connect(m_fileActions.get(), &FileActions::stageClosed,
            this, &MainWindow::onStageClosed);

    // Connect menu manager signals
    connect(m_menuManager.get(), &MenuManager::modeToggleRequested,
            this, &MainWindow::toggleMode);
    connect(m_menuManager.get(), &MenuManager::helpRequested,
            this, &MainWindow::showHelp);
    connect(m_menuManager.get(), &MenuManager::aboutRequested,
            this, &MainWindow::showAbout);
    connect(m_menuManager.get(), &MenuManager::quitRequested,
            this, &MainWindow::quitApp);

    // Connect toolbar manager signals
    connect(m_toolbarManager.get(), &ToolbarManager::modeToggleRequested,
            this, &MainWindow::toggleMode);
    connect(m_toolbarManager.get(), &ToolbarManager::messageLogged,
            m_logManager.get(), &LogManager::logMessage);

    // Connect scene tree to properties widget
    connect(m_sceneTreeWidget, &SceneTreeWidget::primSelected,
            m_primPropertiesWidget, &PrimPropertiesWidget::setPrim);
}

void MainWindow::toggleMode()
{
    m_currentMode = (m_currentMode == ApplicationMode::Edit) ? ApplicationMode::View : ApplicationMode::Edit;
    updateUIForMode();

    QString modeText = (m_currentMode == ApplicationMode::Edit) ? "Edit" : "View";
    m_logManager->logMessage(tr("Switched to %1 mode").arg(modeText));
}

void MainWindow::updateUIForMode()
{
    // Update all UI managers for the new mode
    m_menuManager->updateForMode(m_currentMode);
    m_toolbarManager->updateForMode(m_currentMode);
    m_statusManager->updateForMode(m_currentMode);

    // Update window title
    QString modeText = (m_currentMode == ApplicationMode::Edit) ? "Edit" : "View";
    setWindowTitle(QString("USD Workbench - %1 Mode").arg(modeText));
}

void MainWindow::onStageLoaded()
{
    if (m_stageManager && m_stageManager->HasStage())
    {
        m_sceneTreeWidget->setStage(m_stageManager->GetStage());
    }
}

void MainWindow::onStageClosed()
{
    m_sceneTreeWidget->clear();
}

void MainWindow::showHelp()
{
    QMessageBox::information(this, tr("Help"),
                             tr("Use File > Open to load a USD file.\n"
                                "Use File > Convert to convert between USD/FBX formats."));
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, tr("About USD Workbench"),
                       tr("USD Workbench\nA Qt GUI for USD/FBX conversion."));
}

void MainWindow::quitApp()
{
    QApplication::quit();
}