#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>

namespace Ui {
class TaskDialog;
}

class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDialog(QWidget *parent = nullptr);
    ~TaskDialog();

    QString taskName() const;
    QString taskType() const;
    QString targetType() const;
    QString targetName() const;
    QString assignedUAVs() const;
    QTime startTime() const;
    QTime endTime() const;

    void setTaskName(const QString& name);
    void setTaskType(const QString& type);
    void setTargetType(const QString& type);
    void setTargetName(const QString& name);
    void setAssignedUAVs(const QString& uavs);
    void setStartTime(const QTime& time);
    void setEndTime(const QTime& time);

private slots:
    void onOkClicked();
    void onCancelClicked();

private:
    Ui::TaskDialog *ui;
};

#endif // TASKDIALOG_H