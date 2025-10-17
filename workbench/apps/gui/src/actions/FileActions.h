#pragma once
#include <QObject>
#include <QString>
#include <memory>

class StageManager;
class QWidget;

class FileActions : public QObject
{
    Q_OBJECT

public:
    explicit FileActions(std::unique_ptr<StageManager> &stageManager, QWidget *parent = nullptr);

public slots:
    void openUsdFile();
    void closeStage();
    void convertFile();

signals:
    void messageLogged(const QString &message);
    void stageLoaded();
    void stageClosed();

private:
    std::unique_ptr<StageManager> &m_stageManager;
    QWidget *m_parent;
    QString m_lastOpenedFile;
};