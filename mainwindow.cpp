#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    StatusOfWork = false;
    ui->XORValue->setInputMask(">HH HH HH HH HH HH HH HH");
    ui->PeriodEdit->setValidator(new QIntValidator(1, INT_MAX));

    FinishedFiles = 0;
    AllFiles = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SwitchState()
{
    StatusOfWork = not(StatusOfWork);
    bool not_StatusOfWork = not(StatusOfWork);

    ui->MaskEdit->setEnabled(not_StatusOfWork);
    ui->InDelete->setEnabled(not_StatusOfWork);
    ui->PathEdit->setEnabled(not_StatusOfWork);
    ui->OutEdit->setEnabled(not_StatusOfWork);
    ui->WorkType->setEnabled(not_StatusOfWork);
    if (ui->WorkType->currentIndex() == 1)
    {
        ui->PeriodEdit->setEnabled(not_StatusOfWork);
    }
    ui->XORValue->setEnabled(not_StatusOfWork);
    ui->start->setEnabled(not_StatusOfWork);
}

void MainWindow::StartWork()
{
    ui->progressBar->setValue(0);

    WorkInfo WorkingInfo;
    WorkingInfo.OutPath = ui->PathEdit->toPlainText();
    WorkingInfo.XORValue = QByteArray::fromHex( ui->XORValue->text().replace( " ", "" ).toUtf8());
    WorkingInfo.OutEdit = ui->OutEdit->currentIndex();
    WorkingInfo.DeleteAfter = ui->InDelete->isChecked();
    WorkingInfo.WorkType = ui->WorkType->currentIndex();
    WorkingInfo.TimerTime = ui->PeriodEdit->text().toInt();
    WorkingInfo.FileMask = ui->MaskEdit->toPlainText();

    ProccesWorker = new Worker(WorkingInfo);
    thread = new QThread;

    connect(thread, &QThread::started, ProccesWorker, &Worker::Start);
    connect(ProccesWorker, &Worker::finished, this, &MainWindow::ProcessFinished);
    connect(ProccesWorker, &Worker::finished_cycle, this, [=]{ui->stop->setEnabled(true);});
    connect(ProccesWorker, &Worker::finished_file, this, &MainWindow::UpdateProgress);

    connect(ProccesWorker, &Worker::started_cycle, this, [=]{ui->stop->setEnabled(false);});
    connect(this, &MainWindow::RequestToStopProcess, ProccesWorker, &Worker::StopProcess);

    connect(ProccesWorker, &Worker::finished, thread, &QThread::quit);
    connect(ProccesWorker, &Worker::finished, ProccesWorker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    ProccesWorker->moveToThread(thread);
    thread->start();
}

void MainWindow::ProcessFinished()
{
    if (ui->WorkType->currentIndex() == 0)
    {
        SwitchState();
    }
    else
    {
        ui->stop->setEnabled(false);
    }
}

void MainWindow::UpdateProgress(int value)
{
    ui->progressBar->setValue(value);
}

void MainWindow::on_start_clicked()
{
    SwitchState();
    StartWork();
}


void MainWindow::on_stop_clicked()
{
    SwitchState();
    emit RequestToStopProcess();
}


void MainWindow::on_WorkType_currentIndexChanged(int index)
{
    if (index == 1)
    {
        ui->PeriodEdit->setEnabled(true);
    }
    else
    {
        ui->PeriodEdit->setEnabled(false);
    }
}


void MainWindow::on_PeriodEdit_textChanged(const QString &arg1)
{
    if (arg1.toInt() == 0)
    {
        ui->PeriodEdit->clear();
    }
}

