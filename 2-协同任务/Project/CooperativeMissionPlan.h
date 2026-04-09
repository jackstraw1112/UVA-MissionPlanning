#ifndef CooperativeMissionPlan_H
#define CooperativeMissionPlan_H

#include <QMainWindow>
#include <QTimer>
#include "TaskDialog.h"

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
    void initializeTables(); // 添加函数声明
    void initializeTree();   // 添加函数声明
    
    // 表格数据显示接口
    void DisplayTaskTableToData(const QList<TaskInfo>& taskList);
    void DisplayForceCalculationTableToData(const QList<ForceCalculation>& forceList);
    void DisplayAllocationTableToData(const QList<TaskAllocation>& allocationList);
    void DisplayPathTableToData(const QList<PathPlanning>& pathList);
    void DisplayTargetTableToData(const QList<TargetParam>& targetList);
    
    // 文件操作函数
    bool saveTasksToFile(const QString& filePath);
    bool loadTasksFromFile(const QString& filePath);

private slots:
    void updateClock();
    void onTabButtonClicked();
    void onNewTask();
    void onOpenTask();
    void onSaveTask();
    void onSaveAs();
    void onExport();
    void onEditTask();
    void onDeleteTask();
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
    void onAddTargetParam();
    void onEditTargetParam();
    void onSaveAllocation();
    
    // 接收任务添加信号的槽函数
    void onTaskAdded(const TaskInfo& taskInfo);
    
    // 接收任务更新信号的槽函数
    void onTaskUpdated(int index, const TaskInfo& taskInfo);

private:
    Ui::CooperativeMissionPlan *ui;
    
    // 任务数据列表
    QList<TaskInfo> taskList;
    
    // 兵力需求计算列表
    QList<ForceCalculation> forceList;
    
    // 编组信息列表
    QList<GroupInfo> groupList;
    
    // 当前文件路径
    QString currentFilePath;
    
    // 更新兵力资源管理树
    void updateForceTree();
};

#endif // CooperativeMissionPlan_H