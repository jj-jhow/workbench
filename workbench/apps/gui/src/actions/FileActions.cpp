#include "FileActions.h"
#include "StageManager.h"

// Qt MOC workaround for emit keyword
#ifdef QT_NO_EMIT
#define emit
#endif
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QWidget>
#include <filesystem>
#include "converters/ConverterFactory.h"

namespace fs = std::filesystem;

FileActions::FileActions(std::unique_ptr<StageManager> &stageManager, QWidget *parent)
    : QObject(parent), m_stageManager(stageManager), m_parent(parent)
{
}

void FileActions::openUsdFile()
{
    QString fileName = QFileDialog::getOpenFileName(m_parent,
                                                    tr("Open USD File"),
                                                    QDir::homePath(),
                                                    tr("USD Files (*.usd *.usda *.usdc *.usdz);;All Files (*)"));

    if (fileName.isEmpty())
        return;

    m_lastOpenedFile = fileName;

    if (m_stageManager->LoadStage(fileName.toStdString()))
    {
        emit messageLogged(tr("Loaded USD stage: %1").arg(fileName));
        emit stageLoaded();
    }
    else
    {
        emit messageLogged(tr("Failed to load USD stage: %1").arg(fileName));
        QMessageBox::warning(m_parent, tr("Error"), tr("Failed to load USD stage."));
    }
}

void FileActions::closeStage()
{
    if (m_stageManager->HasStage())
    {
        m_stageManager->ClearStage();
        emit messageLogged(tr("Closed current USD stage."));
        emit stageClosed();
    }
    else
    {
        emit messageLogged(tr("No USD stage to close."));
    }
}

void FileActions::convertFile()
{
    QString inputPath = QFileDialog::getOpenFileName(m_parent,
                                                     tr("Select Input File"),
                                                     QDir::homePath(),
                                                     tr("All Files (*)"));

    if (inputPath.isEmpty())
        return;

    QStringList formats;
    formats << "usd" << "usda" << "usdc" << "fbx";

    bool ok = false;
    QString outputFormat = QInputDialog::getItem(m_parent,
                                                 tr("Select Output Format"),
                                                 tr("Format:"),
                                                 formats, 0, false, &ok);

    if (!ok || outputFormat.isEmpty())
        return;

    QString outputPath = QFileDialog::getSaveFileName(m_parent,
                                                      tr("Select Output File"),
                                                      QDir::homePath(),
                                                      tr("All Files (*)"));

    if (outputPath.isEmpty())
        return;

    auto converter = converters::ConverterFactory::Instance().GetConverterFor(
        inputPath.toStdString(), outputFormat.toStdString());

    if (!converter)
    {
        emit messageLogged(tr("No converter available for this file type and output format."));
        QMessageBox::warning(m_parent, tr("Error"),
                             tr("No converter available for this file type and output format."));
        return;
    }

    if (converter->Convert(fs::path(inputPath.toStdString()),
                           fs::path(outputPath.toStdString()),
                           converters::ConverterOptions()))
    {
        emit messageLogged(tr("Conversion succeeded! Output: %1").arg(outputPath));
    }
    else
    {
        emit messageLogged(tr("Conversion failed."));
        QMessageBox::warning(m_parent, tr("Error"), tr("Conversion failed."));
    }
}