#ifndef ADDFORCEDIALOG_H
#define ADDFORCEDIALOG_H

#include <QDialog>
#include "StructData.h"

namespace Ui {
class AddForceDialog;
}

/**
 * @brief 添加兵力对话框类
 * @details 用于添加新的兵力编组信息到系统中
 */
class AddForceDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针，默认为nullptr
     */
    explicit AddForceDialog(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~AddForceDialog();

signals:
    /**
     * @brief 兵力添加信号
     * @param groupInfo 添加的编组信息
     */
    void forceAdded(const GroupInfo& groupInfo);

private slots:
    /**
     * @brief 确定按钮槽函数
     * @details 验证输入并发送forceAdded信号
     */
    void onOkButtonClicked();

    /**
     * @brief 取消按钮槽函数
     * @details 关闭对话框，不保存数据
     */
    void onCancelButtonClicked();

    /**
     * @brief 关闭按钮槽函数
     * @details 关闭对话框
     */
    void onCloseButtonClicked();

private:
    Ui::AddForceDialog *ui;  // UI界面指针
};

#endif // ADDFORCEDIALOG_H
