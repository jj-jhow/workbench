#include "MainWindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>
#include <QLabel>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QApplication>
#include <QCloseEvent>
#include <QString>
#include <QDir>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <QDockWidget>
#include <QPushButton>

#include "converters/ConverterFactory.h"
#include "SceneViewWidget.h"
#include "SceneTreeWidget.h"
#include "PrimPropertiesWidget.h"

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), stageManager(std::make_unique<StageManager>()), currentMode(ApplicationMode::View)
{
    createMenus();
    createToolBar();
    createStatusBar();
    createLogWindow();
    setWindowTitle("USD Workbench");
    resize(800, 600);

    // Initialize UI for the default mode
    updateUIForMode();

    // --- Splitter-based UI setup ---
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    sceneViewWidget = new SceneViewWidget(mainSplitter);
    rightSplitter = new QSplitter(Qt::Vertical, mainSplitter);
    sceneTreeWidget = new SceneTreeWidget(rightSplitter);
    primPropertiesWidget = new PrimPropertiesWidget(rightSplitter);

    rightSplitter->setStretchFactor(0, 2); // Scene tree gets more space
    rightSplitter->setStretchFactor(1, 1); // Properties less
    mainSplitter->setStretchFactor(0, 3);  // Scene view wider
    mainSplitter->setStretchFactor(1, 2);

    setCentralWidget(mainSplitter);

    connect(sceneTreeWidget, &SceneTreeWidget::primSelected,
            primPropertiesWidget, &PrimPropertiesWidget::setPrim);
}

MainWindow::~MainWindow() {}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    openAct = new QAction(tr("Open USD File..."), this);
    connect(openAct, &QAction::triggered, this, &MainWindow::openUsdFile);
    fileMenu->addAction(openAct);

    convertAct = new QAction(tr("Convert File..."), this);
    connect(convertAct, &QAction::triggered, this, &MainWindow::convertFile);
    fileMenu->addAction(convertAct);

    closeStageAct = new QAction(tr("Close Stage"), this);
    connect(closeStageAct, &QAction::triggered, this, &MainWindow::closeStage);
    fileMenu->addAction(closeStageAct);

    fileMenu->addSeparator();
    exitAct = new QAction(tr("E&xit"), this);
    connect(exitAct, &QAction::triggered, this, &MainWindow::quitApp);
    fileMenu->addAction(exitAct);

    // Mode menu
    modeMenu = menuBar()->addMenu(tr("&Mode"));
    modeToggleAct = new QAction(tr("Switch to Edit Mode"), this);
    connect(modeToggleAct, &QAction::triggered, this, &MainWindow::toggleMode);
    modeMenu->addAction(modeToggleAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpAct = new QAction(tr("Help"), this);
    connect(helpAct, &QAction::triggered, this, &MainWindow::showHelp);
    helpMenu->addAction(helpAct);
    aboutAct = new QAction(tr("About"), this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBar()
{
    mainToolBar = addToolBar(tr("Main Toolbar"));

    // Add mode toggle button to main toolbar
    modeToggleButton = new QPushButton(tr("Switch to Edit Mode"), this);
    modeToggleButton->setCheckable(true);
    connect(modeToggleButton, &QPushButton::clicked, this, &MainWindow::toggleMode);
    mainToolBar->addWidget(modeToggleButton);

    mainToolBar->addSeparator();

    // Create separate toolbars for edit and view modes
    editToolBar = addToolBar(tr("Edit Tools"));
    viewToolBar = addToolBar(tr("View Tools"));

    setupEditModeTools();
    setupViewModeTools();
}

void MainWindow::createStatusBar()
{
    statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);
    statusLabel->setText("Ready");
}

void MainWindow::createLogWindow()
{
    logOutput = new QTextEdit(this);
    logOutput->setReadOnly(true);
    QDockWidget *logDock = new QDockWidget(tr("Log"), this);
    logDock->setWidget(logOutput);
    logDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, logDock);
}

void MainWindow::logMessage(const QString &msg)
{
    logOutput->append(msg);
    statusLabel->setText(msg);
}

void MainWindow::openUsdFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open USD File"), QDir::homePath(), tr("USD Files (*.usd *.usda *.usdc *.usdz);;All Files (*)"));
    if (fileName.isEmpty())
        return;
    lastOpenedFile = fileName;
    if (stageManager->LoadStage(fileName.toStdString()))
    {
        logMessage(tr("Loaded USD stage: %1").arg(fileName));
        sceneTreeWidget->setStage(stageManager->GetStage());
    }
    else
    {
        logMessage(tr("Failed to load USD stage: %1").arg(fileName));
        QMessageBox::warning(this, tr("Error"), tr("Failed to load USD stage."));
    }
}

void MainWindow::closeStage()
{
    if (stageManager->HasStage())
    {
        stageManager->ClearStage();
        sceneTreeWidget->clear();
        logMessage(tr("Closed current USD stage."));
    }
    else
    {
        logMessage(tr("No USD stage to close."));
    }
}

void MainWindow::convertFile()
{
    QString inputPath = QFileDialog::getOpenFileName(this, tr("Select Input File"), QDir::homePath(), tr("All Files (*)"));
    if (inputPath.isEmpty())
        return;
    QStringList formats;
    formats << "usd" << "usda" << "usdc" << "fbx";
    bool ok = false;
    QString outputFormat = QInputDialog::getItem(this, tr("Select Output Format"), tr("Format:"), formats, 0, false, &ok);
    if (!ok || outputFormat.isEmpty())
        return;
    QString outputPath = QFileDialog::getSaveFileName(this, tr("Select Output File"), QDir::homePath(), tr("All Files (*)"));
    if (outputPath.isEmpty())
        return;
    auto converter = converters::ConverterFactory::Instance().GetConverterFor(inputPath.toStdString(), outputFormat.toStdString());
    if (!converter)
    {
        logMessage(tr("No converter available for this file type and output format."));
        QMessageBox::warning(this, tr("Error"), tr("No converter available for this file type and output format."));
        return;
    }
    if (converter->Convert(fs::path(inputPath.toStdString()), fs::path(outputPath.toStdString()), converters::ConverterOptions()))
    {
        logMessage(tr("Conversion succeeded! Output: %1").arg(outputPath));
    }
    else
    {
        logMessage(tr("Conversion failed."));
        QMessageBox::warning(this, tr("Error"), tr("Conversion failed."));
    }
}

void MainWindow::showHelp()
{
    QMessageBox::information(this, tr("Help"), tr("Use File > Open to load a USD file.\nUse File > Convert to convert between USD/FBX formats."));
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, tr("About USD Workbench"), tr("USD Workbench\nA Qt GUI for USD/FBX conversion."));
}

void MainWindow::quitApp()
{
    QApplication::quit();
}

void MainWindow::toggleMode()
{
    currentMode = (currentMode == ApplicationMode::Edit) ? ApplicationMode::View : ApplicationMode::Edit;
    updateUIForMode();

    QString modeText = (currentMode == ApplicationMode::Edit) ? "Edit" : "View";
    logMessage(tr("Switched to %1 mode").arg(modeText));
}

void MainWindow::updateUIForMode()
{
    if (currentMode == ApplicationMode::Edit)
    {
        // Update button and menu text
        modeToggleButton->setText(tr("Switch to View Mode"));
        modeToggleButton->setChecked(true);
        modeToggleAct->setText(tr("Switch to View Mode"));

        // Show edit toolbar, hide view toolbar
        editToolBar->setVisible(true);
        viewToolBar->setVisible(false);

        // Update status
        statusLabel->setText(tr("Edit Mode - Authoring tools available"));
        setWindowTitle("USD Workbench - Edit Mode");
    }
    else
    {
        // Update button and menu text
        modeToggleButton->setText(tr("Switch to Edit Mode"));
        modeToggleButton->setChecked(false);
        modeToggleAct->setText(tr("Switch to Edit Mode"));

        // Show view toolbar, hide edit toolbar
        editToolBar->setVisible(false);
        viewToolBar->setVisible(true);

        // Update status
        statusLabel->setText(tr("View Mode - Viewing tools available"));
        setWindowTitle("USD Workbench - View Mode");
    }
}

void MainWindow::setupEditModeTools()
{
    // Placeholder actions for future authoring tools
    createPrimAct = new QAction(tr("Create Prim"), this);
    createPrimAct->setToolTip(tr("Create a new primitive in the scene"));
    createPrimAct->setEnabled(false); // Disabled until implemented
    connect(createPrimAct, &QAction::triggered, [this]()
            { logMessage(tr("Create Prim tool - Not yet implemented")); });
    editToolBar->addAction(createPrimAct);

    deletePrimAct = new QAction(tr("Delete Prim"), this);
    deletePrimAct->setToolTip(tr("Delete selected primitive"));
    deletePrimAct->setEnabled(false); // Disabled until implemented
    connect(deletePrimAct, &QAction::triggered, [this]()
            { logMessage(tr("Delete Prim tool - Not yet implemented")); });
    editToolBar->addAction(deletePrimAct);

    editToolBar->addSeparator();

    transformToolAct = new QAction(tr("Transform"), this);
    transformToolAct->setToolTip(tr("Transform selected objects"));
    transformToolAct->setEnabled(false); // Disabled until implemented
    connect(transformToolAct, &QAction::triggered, [this]()
            { logMessage(tr("Transform tool - Not yet implemented")); });
    editToolBar->addAction(transformToolAct);

    materialEditorAct = new QAction(tr("Materials"), this);
    materialEditorAct->setToolTip(tr("Edit materials and shading"));
    materialEditorAct->setEnabled(false); // Disabled until implemented
    connect(materialEditorAct, &QAction::triggered, [this]()
            { logMessage(tr("Material Editor - Not yet implemented")); });
    editToolBar->addAction(materialEditorAct);
}

void MainWindow::setupViewModeTools()
{
    // Placeholder actions for future viewing tools
    playAnimationAct = new QAction(tr("Play Animation"), this);
    playAnimationAct->setToolTip(tr("Play/pause animation timeline"));
    playAnimationAct->setEnabled(false); // Disabled until implemented
    connect(playAnimationAct, &QAction::triggered, [this]()
            { logMessage(tr("Animation Player - Not yet implemented")); });
    viewToolBar->addAction(playAnimationAct);

    frameStageAct = new QAction(tr("Frame Stage"), this);
    frameStageAct->setToolTip(tr("Frame the entire stage in the viewport"));
    frameStageAct->setEnabled(false); // Disabled until implemented
    connect(frameStageAct, &QAction::triggered, [this]()
            { logMessage(tr("Frame Stage - Not yet implemented")); });
    viewToolBar->addAction(frameStageAct);

    viewToolBar->addSeparator();

    measureToolAct = new QAction(tr("Measure"), this);
    measureToolAct->setToolTip(tr("Measure distances and angles"));
    measureToolAct->setEnabled(false); // Disabled until implemented
    connect(measureToolAct, &QAction::triggered, [this]()
            { logMessage(tr("Measure tool - Not yet implemented")); });
    viewToolBar->addAction(measureToolAct);

    inspectToolAct = new QAction(tr("Inspect"), this);
    inspectToolAct->setToolTip(tr("Inspect object properties and metadata"));
    inspectToolAct->setEnabled(false); // Disabled until implemented
    connect(inspectToolAct, &QAction::triggered, [this]()
            { logMessage(tr("Inspect tool - Not yet implemented")); });
    viewToolBar->addAction(inspectToolAct);
}
