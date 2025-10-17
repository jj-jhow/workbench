#pragma once
#include <QMainWindow>
#include <memory>
#include <QSplitter>
#include "StageManager.h"
#include "SceneViewWidget.h"
#include "SceneTreeWidget.h"
#include "PrimPropertiesWidget.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QToolBar;
class QLabel;
class QTextEdit;
class QPushButton;
QT_END_NAMESPACE

enum class ApplicationMode
{
    Edit,
    View
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openUsdFile();
    void closeStage();
    void convertFile();
    void showHelp();
    void showAbout();
    void quitApp();
    void toggleMode();

private:
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createLogWindow();
    void logMessage(const QString &msg);
    void updateUIForMode();
    void setupEditModeTools();
    void setupViewModeTools();

    QMenu *fileMenu;
    QMenu *helpMenu;
    QMenu *modeMenu;
    QToolBar *mainToolBar;
    QToolBar *editToolBar;
    QToolBar *viewToolBar;
    QAction *openAct;
    QAction *closeStageAct;
    QAction *convertAct;
    QAction *exitAct;
    QAction *helpAct;
    QAction *aboutAct;
    QAction *modeToggleAct;
    QPushButton *modeToggleButton;
    QLabel *statusLabel;
    QTextEdit *logOutput;

    // Mode management
    ApplicationMode currentMode;

    // Placeholder actions for future authoring tools
    QAction *createPrimAct;
    QAction *deletePrimAct;
    QAction *transformToolAct;
    QAction *materialEditorAct;

    // Placeholder actions for future viewing tools
    QAction *playAnimationAct;
    QAction *frameStageAct;
    QAction *measureToolAct;
    QAction *inspectToolAct;

    std::unique_ptr<StageManager> stageManager;
    QString lastOpenedFile;

    // Splitter-based UI components
    QSplitter *mainSplitter;
    QSplitter *rightSplitter;
    SceneViewWidget *sceneViewWidget;
    SceneTreeWidget *sceneTreeWidget;
    PrimPropertiesWidget *primPropertiesWidget;
};
