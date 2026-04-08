#ifndef STRUCTDATA_H
#define STRUCTDATA_H

#include <QString>
#include <QList>
#include <QTime>

// 任务信息结构体（对应taskTable）
struct TaskInfo {
    QString taskName;          // 任务名称
    QString taskType;          // 任务类型（打击、压制）
    QString targetType;        // 目标类型（点目标、区域目标）
    QString taskTarget;        // 任务目标（目标名称/区域名称）
    QTime startTime;           // 开始时间
    QTime endTime;             // 结束时间
    QString allocatedUAVs;     // 分配无人机（逗号分隔）
    
    // 获取时间范围字符串
    QString getTimeRange() const {
        return startTime.toString("HH:mm") + "~" + endTime.toString("HH:mm");
    }
};

// 兵力需求计算结构体（对应forceCalculationTable）
struct ForceCalculation {
    QString taskName;          // 任务名称
    QString taskTarget;        // 任务目标
    QString threatLevel;       // 威胁等级（高、中、低）
    QString priority;          // 优先级（P1、P2、P3）
    int calculatedCount;       // 计算数量
    int adjustedCount;         // 调整数量
};

// 无人机信息结构体（用于兵力资源管理）
struct UAVInfo {
    QString uavName;           // 无人机名称
    QString status;            // 状态（就绪、待命、故障）
};

// 装备型号结构体（用于兵力资源管理）
struct EquipmentType {
    QString equipmentName;     // 装备型号名称
    QList<UAVInfo> uavList;    // 该型号下的无人机列表
};

// 编组信息结构体（对应forceTree）
struct GroupInfo {
    QString groupName;         // 编组名称
    QList<EquipmentType> equipmentList;  // 该编组下的装备型号列表
    
    // 获取编组中的无人机总数
    int getTotalUAVCount() const {
        int count = 0;
        for (const auto& equipment : equipmentList) {
            count += equipment.uavList.size();
        }
        return count;
    }
    
    // 获取编组中的可用无人机数（就绪状态）
    int getAvailableUAVCount() const {
        int count = 0;
        for (const auto& equipment : equipmentList) {
            for (const auto& uav : equipment.uavList) {
                if (uav.status == "就绪") {
                    count++;
                }
            }
        }
        return count;
    }
};

// 任务分配结构体（对应allocationTable）
struct TaskAllocation {
    QString taskName;          // 任务名称
    QString targetType;        // 目标类型
    QString taskTarget;        // 任务目标
    QString threatLevel;       // 威胁等级
    QString allocatedUAVs;     // 分配无人机（逗号分隔）
    QString formation;         // 编队
};

// 路径规划结构体（对应pathTable）
struct PathPlanning {
    QString uavName;           // 无人机名称
    QString relatedTask;       // 关联任务
    int pathPointCount;        // 路径点数
    QString status;            // 状态（已生成、待生成）
};

// 目标参数结构体（对应targetTable）
struct TargetParam {
    QString targetId;          // 目标编号
    QString frequencyRange;    // 频率范围(MHz)
    QString pulseWidthRange;   // 脉宽范围(μs)
    QString repetitionPeriod;  // 重复周期(ms)
    QString workingMode;       // 工作模式
};

#endif // STRUCTDATA_H