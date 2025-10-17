#include "ToolbarManager.h"
#include <QMainWindow>

// Qt MOC workaround for emit keyword
#ifdef QT_NO_EMIT
#define emit
#endif
#include <QToolBar>
#include <QAction>
#include <QPushButton>

ToolbarManager::ToolbarManager(QMainWindow *mainWindow, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow)
{
    setupMainToolbar();
    setupEditModeTools();
    setupViewModeTools();
}

void ToolbarManager::setupMainToolbar()
{
    m_mainToolBar = m_mainWindow->addToolBar(tr("Main Toolbar"));

    // Add mode toggle button to main toolbar
    m_modeToggleButton = new QPushButton(tr("Switch to Edit Mode"), m_mainWindow);
    m_modeToggleButton->setCheckable(true);
    connect(m_modeToggleButton, &QPushButton::clicked, this, &ToolbarManager::modeToggleRequested);
    m_mainToolBar->addWidget(m_modeToggleButton);

    m_mainToolBar->addSeparator();
}

void ToolbarManager::setupEditModeTools()
{
    m_editToolBar = m_mainWindow->addToolBar(tr("Edit Tools"));

    // Placeholder actions for future authoring tools
    m_createPrimAct = new QAction(tr("Create Prim"), this);
    m_createPrimAct->setToolTip(tr("Create a new primitive in the scene"));
    m_createPrimAct->setEnabled(false); // Disabled until implemented
    connect(m_createPrimAct, &QAction::triggered, [this]()
            { emit messageLogged(tr("Create Prim tool - Not yet implemented")); });
    m_editToolBar->addAction(m_createPrimAct);

    m_deletePrimAct = new QAction(tr("Delete Prim"), this);
    m_deletePrimAct->setToolTip(tr("Delete selected primitive"));
    m_deletePrimAct->setEnabled(false); // Disabled until implemented
    connect(m_deletePrimAct, &QAction::triggered, [this]()
            { emit messageLogged(tr("Delete Prim tool - Not yet implemented")); });
    m_editToolBar->addAction(m_deletePrimAct);

    m_editToolBar->addSeparator();

    m_transformToolAct = new QAction(tr("Transform"), this);
    m_transformToolAct->setToolTip(tr("Transform selected objects"));
    m_transformToolAct->setEnabled(false); // Disabled until implemented
    connect(m_transformToolAct, &QAction::triggered, [this]()
            { emit messageLogged(tr("Transform tool - Not yet implemented")); });
    m_editToolBar->addAction(m_transformToolAct);

    m_materialEditorAct = new QAction(tr("Materials"), this);
    m_materialEditorAct->setToolTip(tr("Edit materials and shading"));
    m_materialEditorAct->setEnabled(false); // Disabled until implemented
    connect(m_materialEditorAct, &QAction::triggered, [this]()
            { emit messageLogged(tr("Material Editor - Not yet implemented")); });
    m_editToolBar->addAction(m_materialEditorAct);
}

void ToolbarManager::setupViewModeTools()
{
    m_viewToolBar = m_mainWindow->addToolBar(tr("View Tools"));

    // Placeholder actions for future viewing tools
    m_playAnimationAct = new QAction(tr("Play Animation"), this);
    m_playAnimationAct->setToolTip(tr("Play/pause animation timeline"));
    m_playAnimationAct->setEnabled(false); // Disabled until implemented
    connect(m_playAnimationAct, &QAction::triggered, [this]()
            { emit messageLogged(tr("Animation Player - Not yet implemented")); });
    m_viewToolBar->addAction(m_playAnimationAct);

    m_frameStageAct = new QAction(tr("Frame Stage"), this);
    m_frameStageAct->setToolTip(tr("Frame the entire stage in the viewport"));
    m_frameStageAct->setEnabled(false); // Disabled until implemented
    connect(m_frameStageAct, &QAction::triggered, [this]()
            { emit messageLogged(tr("Frame Stage - Not yet implemented")); });
    m_viewToolBar->addAction(m_frameStageAct);

    m_viewToolBar->addSeparator();

    m_measureToolAct = new QAction(tr("Measure"), this);
    m_measureToolAct->setToolTip(tr("Measure distances and angles"));
    m_measureToolAct->setEnabled(false); // Disabled until implemented
    connect(m_measureToolAct, &QAction::triggered, [this]()
            { emit messageLogged(tr("Measure tool - Not yet implemented")); });
    m_viewToolBar->addAction(m_measureToolAct);

    m_inspectToolAct = new QAction(tr("Inspect"), this);
    m_inspectToolAct->setToolTip(tr("Inspect object properties and metadata"));
    m_inspectToolAct->setEnabled(false); // Disabled until implemented
    connect(m_inspectToolAct, &QAction::triggered, [this]()
            { emit messageLogged(tr("Inspect tool - Not yet implemented")); });
    m_viewToolBar->addAction(m_inspectToolAct);
}

void ToolbarManager::updateForMode(ApplicationMode mode)
{
    if (mode == ApplicationMode::Edit)
    {
        // Update button text
        m_modeToggleButton->setText(tr("Switch to View Mode"));
        m_modeToggleButton->setChecked(true);

        // Show edit toolbar, hide view toolbar
        m_editToolBar->setVisible(true);
        m_viewToolBar->setVisible(false);
    }
    else
    {
        // Update button text
        m_modeToggleButton->setText(tr("Switch to Edit Mode"));
        m_modeToggleButton->setChecked(false);

        // Show view toolbar, hide edit toolbar
        m_editToolBar->setVisible(false);
        m_viewToolBar->setVisible(true);
    }
}