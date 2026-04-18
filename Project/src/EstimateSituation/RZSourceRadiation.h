//
// Created by admin on "2026.04.09 T 19:19:20".
//

#ifndef RZSOURCERADIATION_H
#define RZSOURCERADIATION_H

#include <QWidget>
#include <QMap>
#include <QStandardItemModel>
#include <QVector>
#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class RZSourceRadiation;
}
QT_END_NAMESPACE

/**
 * @brief 辐射源列表窗口
 * @details 管理雷达、电台、雷达干扰、通信干扰四类数据的模型初始化、显示与切换，以及态势控制功能。
 */
class RZSourceRadiation : public QWidget
{
    Q_OBJECT

public:
    explicit RZSourceRadiation(QWidget *parent = nullptr);
    ~RZSourceRadiation() override;

    void addData(const RadarPerformancePara &data);
    void addData(const SituationControlData &data);

    void updateData(const RadarPerformancePara &data);
    void updateData(const SituationControlData &data);

    void deleteData(const QString &name);

private:
    // 初始化参数
    void initParams();
    // 初始化对象
    void initObject();
    // 关联信号与槽函数
    void initConnect();

    /**
     * @brief 生成测试数据
     * @details 用户生成临时数据
     */
    void generateTestData();

    /**
     * @brief 初始化表格属性
     * @details 设置表格属性，如列数、列宽、行高、表头、数据样式等
     */
    void initTableAttr();

    /**
     * @brief 初始化数据模型
     * @details 仅创建四类数据模型并初始化表头；不在此函数中写入行数据
     */
    void initTableModel();

    /**
     * @brief 显示表格数据
     * @details 用户点击不同辐射源，显示对应的数据
     */
    void onShowTableData();

    /**
     * @brief 显示辐射源数据
     * @details 辐射源数据显示
     */
    void displayData();

    /**
     * @brief 显示雷达辐射源数据
     * @param data 雷达辐射源数据
     * @param row  表格行数，如果为-1，则添加新的行
     */
    void displayData(const RadarPerformancePara &data, int row = -1);

    void writeModelRow(QStandardItemModel *model, const QStringList &columns, int row = -1);

signals:
    /**
     * @brief 控制状态变更信号
     * @param type 控制类型
     * @param enabled 是否启用
     */
    void controlStateChanged(const QString &type, bool enabled);
    
    /**
     * @brief 雷达数据变更信号
     * @param data 雷达性能参数
     */
    void radarDataChanged(const RadarPerformancePara &data);
    
    /**
     * @brief 雷达数据删除信号
     * @param name 雷达名称
     */
    void radarDataDeleted(const QString &name);
    
public slots:
    /**
     * @brief 接收来自威胁评估模块的雷达数据更新
     * @param data 雷达性能参数
     */
    void onRadarDataUpdated(const RadarPerformancePara &data);
    
    /**
     * @brief 接收来自威胁评估模块的雷达数据删除
     * @param name 雷达名称
     */
    void onRadarDataRemoved(const QString &name);

private slots:
    /**
     * @brief 添加雷达
     */
    void onAddRadar();
    
    /**
     * @brief 编辑雷达
     * @param row 行索引
     */
    void onEditRadar(int row);
    
    /**
     * @brief 删除雷达
     * @param row 行索引
     */
    void onDeleteRadar(int row);


private:
    Ui::RZSourceRadiation *ui;

    // 雷达辐射源数据
    QVector<RadarPerformancePara> m_radarSource;

    QMap<QString, QStandardItemModel *> m_mapModel;

    // 态势控制数据
    QVector<SituationControlData> m_controlData;

private:
    void addDataImpl(const RadarPerformancePara &data);
    void updateDataImpl(const RadarPerformancePara &data);
    void addControlDataImpl(const SituationControlData &data);
    void updateControlDataImpl(const SituationControlData &data);

    void deleteDataByName(const QString &name);
    void deleteControlDataByType(const QString &type);

    template <typename T>
    int findIndexByType(const QVector<T> &container, const QString &type)
    {
        for (int i = 0; i < container.size(); ++i)
        {
            if (container.at(i).type == type)
            {
                return i;
            }
        }
        return -1;
    }
};

#endif //RZSOURCERADIATION_H
