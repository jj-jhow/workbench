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

#include "converters/ConverterFactory.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), stageManager(std::make_unique<StageManager>())
{
    createMenus();
    createToolBar();
    createStatusBar();
    createLogWindow();
    setWindowTitle("USD Workbench");
    resize(800, 600);
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

    fileMenu->addSeparator();
    exitAct = new QAction(tr("E&xit"), this);
    connect(exitAct, &QAction::triggered, this, &MainWindow::quitApp);
    fileMenu->addAction(exitAct);

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
    mainToolBar->addAction(openAct);
    mainToolBar->addAction(convertAct);
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
    setCentralWidget(logOutput);
}

void MainWindow::logMessage(const QString &msg)
{
    logOutput->append(msg);
    statusLabel->setText(msg);
}

void MainWindow::openUsdFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open USD File"), QDir::homePath(), tr("USD Files (*.usd *.usda *.usdc);;All Files (*)"));
    if (fileName.isEmpty())
        return;
    lastOpenedFile = fileName;
    if (stageManager->LoadStage(fileName.toStdString()))
    {
        logMessage(tr("Loaded USD stage: %1").arg(fileName));
    }
    else
    {
        logMessage(tr("Failed to load USD stage: %1").arg(fileName));
        QMessageBox::warning(this, tr("Error"), tr("Failed to load USD stage."));
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
    if (converter->Convert(inputPath.toStdString(), outputPath.toStdString()))
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
