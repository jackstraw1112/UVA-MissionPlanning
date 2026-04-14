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

public:
    void loadFormFromTableRow(int row);
    bool parseInputFromTableRow(int row, RadarPerformancePara *outInput, QString *outId, QString *outName,
                                QString *errorMessage) const;
    void ensureSituationDefaultsOnRecord(int row);
    void applyEvaluationToRow(int row, const RadarPerformancePara &in, const QString &id, const QString &name,
                              const RadarThreatAssessResult *precomputed = nullptr);

    void applySubfactorSynthesisToAllRows();

    const QVector<RadarThreatAssessRecord> &threatRecords() const { return m_radarSources; }

    void onSituationConfirm();



private slots:
    void onTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void onThreatTableCellDoubleClicked(int row, int column);
    void refreshSituationDisplay(); // 由 onSituationConfirmed 调用；将面板态势写回当前行
    void onRepresentativeCentersEdited();
    void onSubfactorSynthesisEdited();
    void onRemoveSelectedRows();
    void onClearTable();
    void onCalcAllRadarThreats();

private:
    Ui::RZThreatAssess *ui;

    RadarThreatAssessResult m_lastResult;
    QVector<RadarThreatAssessRecord> m_radarSources;
    bool m_syncingEntityPick = false;

    // 雷达威胁评估参数
    RadarThreatFactors m_subfactor;

    // 雷达威胁评弹窗
    SetRadarThreatAssess *m_setThreatPanel = nullptr;
};

#endif // RZTHREATASSESSMENT_H
