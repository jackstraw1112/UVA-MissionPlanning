#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>
#include <QTime>
#include "StructData.h"

namespace Ui {
class TaskDialog;
}

/**
 * @brief 任务对话框类
 * @details 用于创建和编辑任务信息，支持新增和修改两种模式
 */
class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数（新增模式）
     * @param parent 父窗口指针
     */
    explicit TaskDialog(QWidget *parent = nullptr);

    /**
     * @brief 构造函数（编辑模式）
     * @param editIndex 要编辑的任务索引
     * @param taskInfo 要编辑的任务信息
     * @param parent 父窗口指针
     */
    explicit TaskDialog(int editIndex, const TaskInfo& taskInfo, QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~TaskDialog();

    /**
     * @brief 设置任务信息
     * @param taskInfo 任务信息结构体
     * @details 用于编辑模式，将任务信息填充到表单
     */
    void setTaskInfo(const TaskInfo &taskInfo);

    /**
     * @brief 获取任务信息
     * @return 任务信息结构体
     * @details 返回用户输入的任务信息
     */
    TaskInfo getTaskInfo() const;

    /**
     * @brief 获取编辑索引
     * @return 编辑索引，-1表示新增模式
     */
    int getEditIndex() const;

signals:
    /**
     * @brief 任务添加信号
     * @param taskInfo 新增的任务信息
     */
    void taskAdded(const TaskInfo& taskInfo);

    /**
     * @brief 任务更新信号
     * @param index 要更新的任务索引
     * @param taskInfo 更新后的任务信息
     */
    void taskUpdated(int index, const TaskInfo& taskInfo);

private slots:
    /**
     * @brief 确定按钮槽函数
     * @details 验证输入，发送相应信号并关闭对话框
     */
    void onOkButtonClicked();

    /**
     * @brief 取消按钮槽函数
     * @details 关闭对话框，不保存数据
     */
    void onCancelButtonClicked();

    /**
     * @brief 关闭按钮槽函数
     */
    void on_closeButton_clicked();

private:
    Ui::TaskDialog *ui;          // UI界面指针
    int editIndex;               // 编辑索引，-1表示添加新任务
};

#endif // TASKDIALOG_H
