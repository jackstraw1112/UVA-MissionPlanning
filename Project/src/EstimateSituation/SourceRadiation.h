//
// Created by admin on "2026.04.08 T 15:03:39".
//

#ifndef SOURCERADIATION_H
#define SOURCERADIATION_H

#include <QWidget>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SourceRadiation;
}
QT_END_NAMESPACE

enum class SourceType {
    Radar,
    Radio,
    CommJamming,
    RadarJamming
};

struct ColumnConfig {
    QStringList headers;
};

// 辐射源数据结构体
struct SourceData {
    QString name;           // 辐射源名称
    QString frequency;      // 频率范围
    QString repetition;     // 重复频率
    QString pulseWidth;     // 脉冲宽度
    QString scanMode;       // 扫描方式
    QString threatLevel;    // 威胁等级
};

// 通用数据结构体（赋值为空，可通过接口传值）
struct RadarSourceGenericData {
    QString name;           // 名称
    QStringList values;      // 数据值列表
};

// 辐射源列表
class SourceRadiation : public QWidget
{
    Q_OBJECT

public:
    explicit SourceRadiation(QWidget *parent = nullptr);
    ~SourceRadiation() override;
    
    // 添加数据的公共接口
    void addData(const RadarSourceGenericData& data);
    
    // 添加辐射源数据
    void addRadarData(const SourceData& data);
    void addRadioData(const SourceData& data);
    void addCommJammingData(const SourceData& data);
    void addRadarJammingData(const SourceData& data);
    
    // 批量添加数据，提高性能
    void addRadarDataBatch(const QList<SourceData>& dataList);
    void addRadioDataBatch(const QList<SourceData>& dataList);
    
    // 删除辐射源数据
    void removeData(int row);
    
    // 更新辐射源数据
    void updateData(int row, const SourceData& data);

private slots:
    void onRadarClicked();
    void onRadioClicked();
    void onCommJammingClicked();
    void onRadarJammingClicked();
    void onCustomContextMenuRequested(const QPoint& pos);

private:
    void initUI();
    void setupTable();
    void loadData(SourceType type);
    void setActiveButton(SourceType type);
    ColumnConfig getColumnConfig(SourceType type) const;
    void fillRadarData(QStandardItemModel* model);
    void fillRadioData(QStandardItemModel* model);
    void fillCommJammingData(QStandardItemModel* model);
    void fillRadarJammingData(QStandardItemModel* model);
    void addTableRow(QStandardItemModel* model, int row, const SourceData& data);

private:
    Ui::SourceRadiation *ui;
    QStandardItemModel* m_currentModel;
    QList<SourceData> m_radarData;
    QList<SourceData> m_radioData;
    QList<SourceData> m_commJamData;
    QList<SourceData> m_radarJamData;
};

#endif //SOURCERADIATION_H
