//
// SetThreatAssess：承载「辐射源信息 + 评估结果」等界面；Ui::SetThreatAssess 仅在本类 cpp 中使用，不对外暴露。
//

#ifndef SETTHREATASSESS_H
#define SETTHREATASSESS_H

#include <QString>
#include <QWidget>

#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SetRadarThreatAssess;
}
QT_END_NAMESPACE

namespace ThreatAssessUi
{
    constexpr int kDefaultPositionDefense = 1;
    constexpr int kDefaultRadiationMode = 1;
    double radiationModeMultiplier(int comboIndex);
    double clamp01(double value);
}

class RZThreatAssess;

class SetRadarThreatAssess : public QWidget
{
    Q_OBJECT

    friend RZThreatAssess;

public:
    explicit SetRadarThreatAssess(QWidget *parent = nullptr);
    ~SetRadarThreatAssess() override;

private:
    // 初始化参数
    void initPara();
    // 初始化对象
    void initClass();
    // 初始化连接
    void signalAndSlot();

public:
    // 显示评估数据
    void setDisplay(const RadarThreatAssessRecord &record);
    // 显示评估数据 1-基本信息 2-典型参数
    void displayData(const RadarThreatAssessRecord &record, int flag);
    // 显示评估数据 实时参数
    void displayData(const RadarRealWrokPara &realPara);
    // 显示评估数据 评估因子
    void displayData(const RadarThreatFactors &factors);
    // 显示评估结果 评估结果
    void displayData(const RadarThreatAssessResult &assessment);

    // 清空显示
    void undisplayData();

private:
    // 计算评估结果
    void onCalculate();
    // 确定
    void onConfirm();
    // 退出
    void onCancel();

    // 读取典型值
    RadarTypicalPara readTypicalFromUi() const;
    // 读取子因子
    RadarThreatFactors readSubfactorFromUi() const;

signals:
    void sigEvaluteResult();

private:
    Ui::SetRadarThreatAssess *ui;

    // 评估记录
    RadarThreatAssessRecord m_record;
};

#endif // SETTHREATASSESS_H
