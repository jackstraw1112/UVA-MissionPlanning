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

/**
 * @brief 协同任务规划主窗口类
 * @details 应用程序的主界面类，负责协调和管理所有任务规划相关的功能和界面
 */
class CooperativeMissionPlan : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    CooperativeMissionPlan(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~CooperativeMissionPlan();

    QTimer *clockTimer;         // 系统时钟定时器
    int selectedTaskIndex;      // 当前选中的任务索引

    /**
     * @brief 设置信号槽连接
     */
    void setupConnections();

    /**
     * @brief 设置界面样式
     */
    void setupStyles();

    /**
     * @brief 显示提示消息
     * @param message 消息内容
     */
    void showToast(const QString& message);

    // ========== 表格数据显示接口 ==========

    /**
     * @brief 显示任务列表到表格
     * @param taskList 任务信息列表
     */
    void DisplayTaskTableToData(const QList<TaskInfo>& taskList);

    /**
     * @brief 显示兵力需求计算列表到表格
     * @param forceList 兵力需求列表
     */
    void DisplayForceCalculationTableToData(const QList<ForceCalculation>& forceList);

    /**
     * @brief 显示任务分配列表到表格
     * @param allocationList 任务分配列表
     */
    void DisplayAllocationTableToData(const QList<TaskAllocation>& allocationList);

    /**
     * @brief 显示路径规划列表到表格
     * @param pathList 路径规划列表
     */
    void DisplayPathTableToData(const QList<PathPlanning>& pathList);

    /**
     * @brief 显示编组信息到兵力资源管理树
     * @param groupList 编组信息列表
     */
    void DisplayForceTreeToData(const QList<GroupInfo>& groupList);

    // ========== 目标参数表格显示接口 ==========

    /**
     * @brief 显示雷达目标参数列表到表格
     * @param radarList 雷达目标参数列表
     */
    void DisplayRadarTargetTableToData(const QList<RadarTargetParam>& radarList);

    /**
     * @brief 显示无线电目标参数列表到表格
     * @param radioList 无线电目标参数列表
     */
    void DisplayRadioTargetTableToData(const QList<RadioTargetParam>& radioList);

    /**
     * @brief 显示通信对抗参数列表到表格
     * @param commList 通信对抗参数列表
     */
    void DisplayCommJammingTableToData(const QList<CommJammingParam>& commList);

    /**
     * @brief 显示雷达对抗参数列表到表格
     * @param rcmList 雷达对抗参数列表
     */
    void DisplayRcmJammingTableToData(const QList<RcmJammingParam>& rcmList);

    // ========== 文件操作函数 ==========

    /**
     * @brief 保存任务到文件
     * @param filePath 文件路径
     * @return 保存是否成功
     */
    bool saveTasksToFile(const QString& filePath);

    /**
     * @brief 从文件加载任务
     * @param filePath 文件路径
     * @return 加载是否成功
     */
    bool loadTasksFromFile(const QString& filePath);

private slots:
    /**
     * @brief 更新系统时钟
     */
    void updateClock();

    /**
     * @brief 新建任务
     */
    void onNewTask();

    /**
     * @brief 打开任务
     */
    void onOpenTask();

    /**
     * @brief 保存任务
     */
    void onSaveTask();

    /**
     * @brief 另存为
     */
    void onSaveAs();

    /**
     * @brief 导出数据
     */
    void onExport();

    /**
     * @brief 编辑任务
     */
    void onEditTask();

    /**
     * @brief 删除任务
     */
    void onDeleteTask();

    /**
     * @brief 打开任务规划管理器
     */
    void onOpenTaskPlanManager();

    /**
     * @brief 计算兵力需求
     */
    void onCalculateForce();

    /**
     * @brief 自动分配任务
     */
    void onAutoAllocate();

    /**
     * @brief 生成路径
     */
    void onGeneratePath();

    /**
     * @brief 绑定参数
     */
    void onBindParameters();

    /**
     * @brief 打开设置
     */
    void onSettings();

    /**
     * @brief 任务选择处理
     * @param index 任务索引
     */
    void onTaskSelected(int index);

    /**
     * @brief 路径标签页切换
     * @param index 标签页索引
     */
    void onPathTabChanged(int index);

    /**
     * @brief 目标标签页切换
     * @param index 标签页索引
     */
    void onTargetTabChanged(int index);

    /**
     * @brief 旋钮值变化处理
     * @param id 旋钮ID
     * @param delta 变化量
     */
    void onSpinValueChanged(int id, int delta);

    /**
     * @brief 树形项展开/折叠处理
     * @param path 树形项路径
     * @param expanded 是否展开
     */
    void onTreeItemToggled(const QString& path, bool expanded);

    /**
     * @brief 关机操作
     */
    void onShutdown();

    /**
     * @brief 添加兵力
     */
    void onAddForce();

    /**
     * @brief 删除兵力
     */
    void onDeleteForce();

    /**
     * @brief 生成选中的路径
     */
    void onGenerateSelectedPath();

    /**
     * @brief 生成所有路径
     */
    void onGenerateAllPaths();

    /**
     * @brief 显示路径详情
     */
    void onShowPathDetail();

    /**
     * @brief 添加目标参数
     */
    void onAddTargetParam();

    /**
     * @brief 编辑目标参数
     */
    void onEditTargetParam();

    /**
     * @brief 保存任务分配
     */
    void onSaveAllocation();

    // ========== 信号处理槽函数 ==========

    /**
     * @brief 任务添加处理
     * @param taskInfo 添加的任务信息
     */
    void onTaskAdded(const TaskInfo& taskInfo);

    /**
     * @brief 任务更新处理
     * @param index 更新的任务索引
     * @param taskInfo 更新后的任务信息
     */
    void onTaskUpdated(int index, const TaskInfo& taskInfo);

    /**
     * @brief 兵力添加处理
     * @param groupInfo 添加的编组信息
     */
    void onForceAdded(const GroupInfo& groupInfo);

    // ========== 目标参数信号处理 ==========

    /**
     * @brief 雷达目标添加处理
     */
    void onRadarTargetAdded(const RadarTargetParam& radar);

    /**
     * @brief 无线电目标添加处理
     */
    void onRadioTargetAdded(const RadioTargetParam& radio);

    /**
     * @brief 通信对抗添加处理
     */
    void onCommJammingAdded(const CommJammingParam& comm);

    /**
     * @brief 雷达对抗添加处理
     */
    void onRcmJammingAdded(const RcmJammingParam& rcm);

    /**
     * @brief 雷达目标更新处理
     */
    void onRadarTargetUpdated(const RadarTargetParam& radar, int index);

    /**
     * @brief 无线电目标更新处理
     */
    void onRadioTargetUpdated(const RadioTargetParam& radio, int index);

    /**
     * @brief 通信对抗更新处理
     */
    void onCommJammingUpdated(const CommJammingParam& comm, int index);

    /**
     * @brief 雷达对抗更新处理
     */
    void onRcmJammingUpdated(const RcmJammingParam& rcm, int index);

    /**
     * @brief 任务规划选中处理
     * @param taskPlan 选中的任务规划
     */
    void onTaskPlanSelected(const TaskInfo& taskPlan);

private:
    Ui::CooperativeMissionPlan *ui;  // UI界面指针

    // ========== 数据列表 ==========

    QList<TaskInfo> taskList;              // 任务数据列表
    QList<ForceCalculation> forceList;     // 兵力需求计算列表
    QList<GroupInfo> groupList;            // 编组信息列表
    QList<PathPlanning> cruisePathList;     // 巡航路径列表
    QList<PathPlanning> searchPathList;     // 搜索路径列表
    QList<TaskAllocation> allocationList;   // 任务分配列表
    QList<RadarTargetParam> radarTargetList;    // 雷达目标参数列表
    QList<RadioTargetParam> radioTargetList;    // 电台目标参数列表
    QList<CommJammingParam> commJammingList;     // 通信对抗参数列表
    QList<RcmJammingParam> rcmJammingList;       // 雷达对抗参数列表

    // ========== 状态变量 ==========

    QString currentFilePath;        // 当前文件路径
    QString currentPlanName;        // 当前方案名称
    QString currentCoordinationName; // 当前协同规划名称

};

#endif // CooperativeMissionPlan_H
