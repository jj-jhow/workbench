#pragma once
#include <QObject>
#include "core/ApplicationMode.h"

QT_BEGIN_NAMESPACE
class QMainWindow;
class QMenu;
class QAction;
QT_END_NAMESPACE

class FileActions;

class MenuManager : public QObject
{
    Q_OBJECT

public:
    explicit MenuManager(QMainWindow *mainWindow, FileActions *fileActions, QObject *parent = nullptr);

    void updateForMode(ApplicationMode mode);

signals:
    void modeToggleRequested();
    void helpRequested();
    void aboutRequested();
    void quitRequested();

private:
    void createFileMenu();
    void createModeMenu();
    void createHelpMenu();

    QMainWindow *m_mainWindow;
    FileActions *m_fileActions;

    // Menus
    QMenu *m_fileMenu;
    QMenu *m_modeMenu;
    QMenu *m_helpMenu;

    // Actions
    QAction *m_openAct;
    QAction *m_convertAct;
    QAction *m_closeStageAct;
    QAction *m_exitAct;
    QAction *m_modeToggleAct;
    QAction *m_helpAct;
    QAction *m_aboutAct;
};