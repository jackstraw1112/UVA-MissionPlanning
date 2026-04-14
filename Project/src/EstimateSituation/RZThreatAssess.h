//
// Created by admin on "2026.04.11 T 13:41:18".
//
// 雷达威胁评估：威胁列表见 RZThreatAssess.ui；辐射源信息与评估结果为 SetThreatAssess，默认隐藏，双击表格行以弹窗打开。
// 数据结构见 EstimateSituationStruct.h。
//

#ifndef RZTHREATASSESSMENT_H
#define RZTHREATASSESSMENT_H

#include "EstimateSituationStruct.h"

#include <QVector>
#include <QWidget>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class RZThreatAssess;
}
QT_END_NAMESPACE

class SetRadarThreatAssess;

/**
 * @brief 雷达威胁评估窗口
 */
class RZThreatAssess : public QWidget
{
    Q_OBJECT

public:
    explicit RZThreatAssess(QWidget *parent = nullptr);
    ~RZThreatAssess() override;

private:
    // 初始化参数
    void initPara();
    // 初始化对象
    void initClass();
    // 初始化连接
    void signalAndSlot();

    // 初始化表格属性
    void initTableAttr();
    // 初始化表格表头
    void initTableHeader();
    // 生成测试数据
    void generateTestData();

    // 显示数据到表格
    void displayDataToTable();
    // 显示数据到表格
    void displayDataToTable(const RadarThreatAssessRecord &data, int row = -1);
    // 显示评估结果
    void displayDataToTable(int row);

    // 评估雷达威胁
    void assessRadarThreat();
    // 双击表格进行评估
    void onTableDoubleClicked(int row, int column);
    // 接收评估结果
    void onRecvAssessResult();

    // 删除选中行
    void onRemoveSelectedRows();
    // 清空表格
    void onClearTable();
    // 批量评估
    void onCalculateAllRadarThreats();
    // 保护实时参数
    void protectRealWorkPara(int row);

private:
    Ui::RZThreatAssess *ui;

    // 雷达评估记录
    QVector<RadarThreatAssessRecord> m_radarSources;

    // 雷达威胁评弹窗
    SetRadarThreatAssess *m_setThreatPanel = nullptr;

    // 雷达威胁评估因子
    RadarThreatFactors m_radarfactor;

    // 选择行数
    int m_selectRow;
    // 雷达实时参数
    RadarRealWrokPara m_workPara;
    // 雷达典型参数
    RadarTypicalPara m_typicalPara;
    // 雷达评估结果
    RadarThreatAssessResult m_result;
};

#endif // RZTHREATASSESSMENT_H
