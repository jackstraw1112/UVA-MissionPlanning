#ifndef CooperativeMissionPlan_H
#define CooperativeMissionPlan_H

#include <QMainWindow>
#include <QTimer>
#include "TaskDialog.h"
#include "AddForceDialog.h"
#include "TargetParamDialog.h"
#include "PathDisplayDialog.h"
#include "TaskPlanManagerDialog.h"
#include "DatabaseManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CooperativeMissionPlan; }
QT_END_NAMESPACE

class CooperativeMissionPlan : public QMainWindow
{
    Q_OBJECT

public:
    CooperativeMissionPlan(QWidget *parent = nullptr);
    ~CooperativeMissionPlan();

    QTimer *clockTimer;
    int selectedTaskIndex;

    void setupConnections();
    void setupStyles();
    void showToast(const QString& message);
    
    // 表格数据显示接口
    void DisplayTaskTableToData(const QList<TaskInfo>& taskList);
    void DisplayForceCalculationTableToData(const QList<ForceCalculation>& forceList);
    void DisplayAllocationTableToData(const QList<TaskAllocation>& allocationList);
    void DisplayPathTableToData(const QList<PathPlanning>& pathList);

    // 更新兵力资源管理树
    void DisplayForceTreeToData(const QList<GroupInfo>& groupList);

    // 目标参数表格显示接口
    void DisplayRadarTargetTableToData(const QList<RadarTargetParam>& radarList);
    void DisplayRadioTargetTableToData(const QList<RadioTargetParam>& radioList);
    void DisplayCommJammingTableToData(const QList<CommJammingParam>& commList);
    void DisplayRcmJammingTableToData(const QList<RcmJammingParam>& rcmList);
    
    // 文件操作函数
    bool saveTasksToFile(const QString& filePath);
    bool loadTasksFromFile(const QString& filePath);

private slots:
    void updateClock();
    void onNewTask();
    void onOpenTask();
    void onSaveTask();
    void onSaveAs();
    void onExport();
    void onEditTask();
    void onDeleteTask();
    void onOpenTaskPlanManager();
    void onCalculateForce();
    void onAutoAllocate();
    void onGeneratePath();
    void onBindParameters();
    void onSettings();
    void onTaskSelected(int index);
    void onPathTabChanged(int index);
    void onTargetTabChanged(int index);
    void onSpinValueChanged(int id, int delta);
    void onTreeItemToggled(const QString& path, bool expanded);
    void onShutdown();
    void onAddForce();
    void onDeleteForce();
    void onGenerateSelectedPath();
    void onGenerateAllPaths();
    void onShowPathDetail();
    void onAddTargetParam();
    void onEditTargetParam();
    void onSaveAllocation();
    
    // 接收任务添加信号的槽函数
    void onTaskAdded(const TaskInfo& taskInfo);
    
    // 接收任务更新信号的槽函数
    void onTaskUpdated(int index, const TaskInfo& taskInfo);
    
    // 接收兵力添加信号的槽函数
    void onForceAdded(const GroupInfo& groupInfo);

    // 接收目标参数添加信号的槽函数
    void onRadarTargetAdded(const RadarTargetParam& radar);
    void onRadioTargetAdded(const RadioTargetParam& radio);
    void onCommJammingAdded(const CommJammingParam& comm);
    void onRcmJammingAdded(const RcmJammingParam& rcm);

    // 接收目标参数更新信号的槽函数
    void onRadarTargetUpdated(const RadarTargetParam& radar, int index);
    void onRadioTargetUpdated(const RadioTargetParam& radio, int index);
    void onCommJammingUpdated(const CommJammingParam& comm, int index);
    void onRcmJammingUpdated(const RcmJammingParam& rcm, int index);

    void onTaskPlanSelected(const TaskInfo& taskPlan);

private:
    Ui::CooperativeMissionPlan *ui;

    // 任务数据列表
    QList<TaskInfo> taskList;

    // 兵力需求计算列表
    QList<ForceCalculation> forceList;

    // 编组信息列表
    QList<GroupInfo> groupList;

    // 巡航路径列表
    QList<PathPlanning> cruisePathList;

    // 搜索路径列表
    QList<PathPlanning> searchPathList;

    // 任务分配列表
    QList<TaskAllocation> allocationList;

    // 雷达目标参数列表
    QList<RadarTargetParam> radarTargetList;

    // 电台目标参数列表
    QList<RadioTargetParam> radioTargetList;

    // 通信对抗参数列表
    QList<CommJammingParam> commJammingList;

    // 雷达对抗参数列表
    QList<RcmJammingParam> rcmJammingList;

    // 当前文件路径
    QString currentFilePath;

    // 当前方案名称和协同规划名称
    QString currentPlanName;
    QString currentCoordinationName;

};

#endif // CooperativeMissionPlan_H