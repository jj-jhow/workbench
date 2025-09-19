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
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openUsdFile();
    void convertFile();
    void showHelp();
    void showAbout();
    void quitApp();

private:
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createLogWindow();
    void logMessage(const QString &msg);

    QMenu *fileMenu;
    QMenu *helpMenu;
    QToolBar *mainToolBar;
    QAction *openAct;
    QAction *convertAct;
    QAction *exitAct;
    QAction *helpAct;
    QAction *aboutAct;
    QLabel *statusLabel;
    QTextEdit *logOutput;

    std::unique_ptr<StageManager> stageManager;
    QString lastOpenedFile;

    // Splitter-based UI components
    QSplitter *mainSplitter;
    QSplitter *rightSplitter;
    SceneViewWidget *sceneViewWidget;
    SceneTreeWidget *sceneTreeWidget;
    PrimPropertiesWidget *primPropertiesWidget;
};
