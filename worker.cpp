#include "worker.h"
#include "qdir.h"
#include "qdiriterator.h"
#include "qtimer.h"

Worker::Worker(WorkInfo WorkingInfo, QObject *parent)
    : QObject{parent}
{
    this->WorkingInfo = WorkingInfo;
    FinishedFiles = 0;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Worker::Start);
}

Worker::~Worker(){
    delete timer;
}

void Worker::Start()
{
    qInfo() << "Начинаем";
    QDir Directory;
    QStringList Masks;
    QString filemask = WorkingInfo.FileMask;
    if (!filemask.isEmpty())
        Masks << filemask;
    QDirIterator FilesToDo(Directory.currentPath(), Masks, QDir::Files);
    QStringList files;
    while (FilesToDo.hasNext())
        files << FilesToDo.next();

    StartProccesing(files);
}

void Worker::StartProccesing(QStringList& FilesList)
{
    FinishedFiles = 0;

    if (WorkingInfo.WorkType == 1)
    {
        emit finished_file(0);
        emit started_cycle();
    }

    for ( const auto& filePath : std::as_const(FilesList) )
    {
        qInfo() << filePath;
        ProccesFile(filePath);
        FinishedFiles++;
        emit finished_file((int)(FinishedFiles*100/FilesList.size()));
    }

    if (WorkingInfo.WorkType == 1)
    {
        timer->start(WorkingInfo.TimerTime * 1000);
        emit finished_cycle();
    }
    else
    {
        emit finished();
    }

}

void Worker::ProccesFile(QString PathFile)
{
    QByteArray XORValue = WorkingInfo.XORValue;
    // Procces of single file
    QFile inputFile(PathFile);
    QFileInfo FileInfo(inputFile);
    inputFile.open(QIODevice::ReadOnly);

    QString path(WorkingInfo.OutPath);
    QFile outFile;
    outFile.setFileName(path + FileInfo.fileName() + "_Output.bin");
    if (WorkingInfo.OutEdit == 1)
    {
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    else if (WorkingInfo.OutEdit == 2)
    {
        if (outFile.exists())
        {
            QStringList matchingFiles = QDir(path).entryList(QStringList() << FileInfo.fileName() + "_Output*.bin", QDir::Files);
            int count = matchingFiles.size();
            outFile.setFileName(path + FileInfo.fileName() + "_Output_" + QString::number(count+1) + ".bin");
            outFile.open(QIODevice::WriteOnly);
        }
        else
        {
            outFile.open(QIODevice::WriteOnly);
        }
    }
    else
    {
        outFile.open(QIODevice::WriteOnly);
    }


    const qint64 bufferSize = 1024 * 1024;
    QByteArray buffer;

    while (!inputFile.atEnd()) {
        buffer = inputFile.read(bufferSize);
        for (int i = 0; i < buffer.size(); ++i)
        {
            buffer[i] = buffer[i] ^ XORValue[i % XORValue.size()];
        }
        outFile.write(buffer);
    }
    outFile.close();
    inputFile.close();

    if (WorkingInfo.DeleteAfter)
    {
        inputFile.remove();
    }
}

void Worker::StopProcess()
{
    timer->stop();
    emit finished();
}
