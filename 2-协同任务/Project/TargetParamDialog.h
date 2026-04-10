#ifndef TARGETPARAMDIALOG_H
#define TARGETPARAMDIALOG_H

#include <QDialog>
#include "StructData.h"

namespace Ui {
class TargetParamDialog;
}

/**
 * @brief 目标参数对话框类
 * @details 用于管理雷达目标、无线电目标、通信干扰、雷达干扰等多种目标参数的配置
 */
class TargetParamDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit TargetParamDialog(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~TargetParamDialog();

    int m_editIndex;               // 编辑索引，-1表示新增
    bool m_isEditMode;             // 是否为编辑模式
    QString m_planName;             // 方案名称
    QString m_coordinationName;     // 协同规划名称

    /**
     * @brief 设置方案信息
     * @param planName 方案名称
     * @param coordinationName 协同规划名称
     */
    void setPlanInfo(const QString& planName, const QString& coordinationName);

    /**
     * @brief 设置雷达目标参数
     * @param radar 雷达目标参数
     */
    void setRadarTarget(const RadarTargetParam& radar);

    /**
     * @brief 设置无线电目标参数
     * @param radio 无线电目标参数
     */
    void setRadioTarget(const RadioTargetParam& radio);

    /**
     * @brief 设置通信干扰参数
     * @param comm 通信干扰参数
     */
    void setCommJamming(const CommJammingParam& comm);

    /**
     * @brief 设置雷达干扰参数
     * @param rcm 雷达干扰参数
     */
    void setRcmJamming(const RcmJammingParam& rcm);

signals:
    /**
     * @brief 雷达目标添加信号
     */
    void radarTargetAdded(const RadarTargetParam& radar);

    /**
     * @brief 雷达目标更新信号
     */
    void radarTargetUpdated(const RadarTargetParam& radar, int index);

    /**
     * @brief 无线电目标添加信号
     */
    void radioTargetAdded(const RadioTargetParam& radio);

    /**
     * @brief 无线电目标更新信号
     */
    void radioTargetUpdated(const RadioTargetParam& radio, int index);

    /**
     * @brief 通信干扰添加信号
     */
    void commJammingAdded(const CommJammingParam& comm);

    /**
     * @brief 通信干扰更新信号
     */
    void commJammingUpdated(const CommJammingParam& comm, int index);

    /**
     * @brief 雷达干扰添加信号
     */
    void rcmJammingAdded(const RcmJammingParam& rcm);

    /**
     * @brief 雷达干扰更新信号
     */
    void rcmJammingUpdated(const RcmJammingParam& rcm, int index);

private slots:
    /**
     * @brief 目标类型切换槽函数
     * @param index 切换到的索引
     */
    void onTargetTypeChanged(int index);

    /**
     * @brief 确定按钮槽函数
     */
    void onOkButtonClicked();

    /**
     * @brief 取消按钮槽函数
     */
    void onCancelButtonClicked();

    /**
     * @brief 关闭按钮槽函数
     */
    void onCloseButtonClicked();

private:
    Ui::TargetParamDialog *ui;  // UI界面指针

};

#endif // TARGETPARAMDIALOG_H
