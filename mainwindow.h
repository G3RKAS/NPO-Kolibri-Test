#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Worker;
class QThread;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_start_clicked();
    void on_stop_clicked();
    void on_WorkType_currentIndexChanged(int index);

    void ProcessFinished();
    void UpdateProgress(int);

    void on_PeriodEdit_textChanged(const QString &arg1);

private:
    Worker* ProccesWorker;
    QThread *thread;
    Ui::MainWindow *ui;
    bool StatusOfWork;

    int FinishedFiles;
    int AllFiles;

    void SwitchState();
    void StartWork();

signals:
    void RequestToStopProcess();
};
#endif // MAINWINDOW_H
