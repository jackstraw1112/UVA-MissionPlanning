#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>
#include <QTime>
#include "StructData.h"

namespace Ui {
class TaskDialog;
}

class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDialog(QWidget *parent = nullptr);
    explicit TaskDialog(int editIndex, const TaskInfo& taskInfo, QWidget *parent = nullptr);
    ~TaskDialog();

    
    // 使用结构体设置表单数据
    void setTaskInfo(const TaskInfo &taskInfo);
    
    // 获取完整的任务信息结构体
    TaskInfo getTaskInfo() const;
    
    // 获取编辑索引
    int getEditIndex() const;

 signals:
    // 发送任务信息信号（添加新任务）
    void taskAdded(const TaskInfo& taskInfo);
    
    // 发送任务信息信号（编辑现有任务）
    void taskUpdated(int index, const TaskInfo& taskInfo);

private slots:
    void onOkButtonClicked();
    void onCancelButtonClicked();
    void on_closeButton_clicked();

private:
    Ui::TaskDialog *ui;
    int editIndex; // 编辑索引，-1表示添加新任务
};

#endif // TASKDIALOG_H