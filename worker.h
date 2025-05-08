#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class QTimer;

struct WorkInfo
{
    QString FileMask;
    QString OutPath;
    QByteArray XORValue;
    int OutEdit;
    bool DeleteAfter;
    int WorkType;
    int TimerTime;
};

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(WorkInfo, QObject *parent = nullptr);
    ~Worker();

    void Start();

private:
    void ProccesFile(QString);
    void StartProccesing(QStringList&);

    QTimer* timer;
    int FinishedFiles;

    WorkInfo WorkingInfo;
    QStringList FilesList;

signals:
    void finished();
    void finished_cycle();
    void started_cycle();
    void finished_file(int);

public slots:
    void StopProcess();
};

#endif // WORKER_H
