#pragma once
#include <QObject>
#include "core/ApplicationMode.h"

QT_BEGIN_NAMESPACE
class QMainWindow;
class QToolBar;
class QAction;
class QPushButton;
QT_END_NAMESPACE

class ToolbarManager : public QObject
{
    Q_OBJECT

public:
    explicit ToolbarManager(QMainWindow *mainWindow, QObject *parent = nullptr);

    void updateForMode(ApplicationMode mode);

signals:
    void modeToggleRequested();
    void messageLogged(const QString &message);

private:
    void setupMainToolbar();
    void setupEditModeTools();
    void setupViewModeTools();

    QMainWindow *m_mainWindow;

    // Toolbars
    QToolBar *m_mainToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_viewToolBar;

    // Main toolbar widgets
    QPushButton *m_modeToggleButton;

    // Edit mode actions
    QAction *m_createPrimAct;
    QAction *m_deletePrimAct;
    QAction *m_transformToolAct;
    QAction *m_materialEditorAct;

    // View mode actions
    QAction *m_playAnimationAct;
    QAction *m_frameStageAct;
    QAction *m_measureToolAct;
    QAction *m_inspectToolAct;
};