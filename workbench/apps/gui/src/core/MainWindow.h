#pragma once
#include <QMainWindow>
#include <QSplitter>
#include <memory>
#include "core/ApplicationMode.h"

class StageManager;
class SceneViewWidget;
class SceneTreeWidget;
class PrimPropertiesWidget;
class FileActions;
class MenuManager;
class ToolbarManager;
class StatusManager;
class LogManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void toggleMode();
    void showHelp();
    void showAbout();
    void quitApp();
    void onStageLoaded();
    void onStageClosed();

private:
    void setupUI();
    void setupConnections();
    void updateUIForMode();

    // Core data
    std::unique_ptr<StageManager> m_stageManager;
    ApplicationMode m_currentMode;

    // UI Managers
    std::unique_ptr<FileActions> m_fileActions;
    std::unique_ptr<MenuManager> m_menuManager;
    std::unique_ptr<ToolbarManager> m_toolbarManager;
    std::unique_ptr<StatusManager> m_statusManager;
    std::unique_ptr<LogManager> m_logManager;

    // Main UI widgets
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;
    SceneViewWidget *m_sceneViewWidget;
    SceneTreeWidget *m_sceneTreeWidget;
    PrimPropertiesWidget *m_primPropertiesWidget;
};