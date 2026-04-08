#ifndef CooperativeMissionPlan_H
#define CooperativeMissionPlan_H

#include <QMainWindow>
#include <QTimer>

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

private:
    Ui::CooperativeMissionPlan *ui;
};

#endif // CooperativeMissionPlan_H