#ifndef STRUCTDATA_H
#define STRUCTDATA_H

#include <QString>
#include <QList>
#include <QTime>

// 任务信息结构体（对应taskTable）
struct TaskInfo {
    QString planName;           // 方案名称
    QString coordinationName;    // 协同规划名称
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

    // 重载==操作符
    bool operator==(const TaskInfo& other) const {
        return planName == other.planName &&
               coordinationName == other.coordinationName &&
               taskName == other.taskName &&
               taskType == other.taskType &&
               targetType == other.targetType &&
               taskTarget == other.taskTarget &&
               startTime == other.startTime &&
               endTime == other.endTime &&
               allocatedUAVs == other.allocatedUAVs;
    }
};

// 兵力需求计算结构体（对应forceCalculationTable）
struct ForceCalculation {
    QString planName;           // 方案名称
    QString coordinationName;    // 协同规划名称
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
    QString planName;           // 方案名称
    QString coordinationName;    // 协同规划名称
    QString taskName;          // 任务名称
    QString targetType;        // 目标类型
    QString taskTarget;        // 任务目标
    QString threatLevel;       // 威胁等级
    QString allocatedUAVs;     // 分配无人机（逗号分隔）
    QString formation;         // 编队
};

// 路径点结构体
struct PathPoint {
    double latitude;      // 纬度
    double longitude;      // 经度
    double altitude;       // 高度(m)
    int pointOrder;        // 点序号
};

// 路径规划结构体（对应pathTable）
struct PathPlanning {
    QString planName;           // 方案名称
    QString coordinationName;    // 协同规划名称
    QString uavName;           // 无人机名称
    QString relatedTask;       // 关联任务
    int pathPointCount;        // 路径点数
    QString status;            // 状态（已生成、待生成）
    QList<PathPoint> pathPoints; // 路径点列表
};

// 雷达目标参数结构体
struct RadarTargetParam {
    QString planName;           // 方案名称
    QString coordinationName;    // 协同规划名称
    QString targetId;          // 目标编号
    QString frequencyRange;    // 频率范围(MHz)
    QString pulseWidthRange;   // 脉宽范围(μs)
    QString repetitionPeriod;  // 重复周期(ms)
    QString workingMode;       // 工作模式
};

// 电台目标参数结构体
struct RadioTargetParam {
    QString planName;           // 方案名称
    QString coordinationName;    // 协同规划名称
    QString targetId;          // 目标编号
    QString frequencyRange;    // 频率范围(MHz)
    QString modulationMode;    // 调制方式
    QString signalBandwidth;   // 信号带宽(kHz)
    QString transmitPower;     // 发射功率(kW)
};

// 通信对抗参数结构体
struct CommJammingParam {
    QString planName;           // 方案名称
    QString coordinationName;    // 协同规划名称
    QString targetId;          // 目标编号
    QString jammingFrequency;  // 干扰频率(MHz)
    QString jammingMode;       // 干扰样式
    QString jammingPower;      // 干扰功率(kW)
    QString coverageRange;     // 覆盖范围(km)
};

// 雷达对抗参数结构体
struct RcmJammingParam {
    QString planName;           // 方案名称
    QString coordinationName;    // 协同规划名称
    QString targetId;          // 目标编号
    QString jammingFrequency;  // 干扰频率(GHz)
    QString jammingMode;       // 干扰样式
    QString jammingPower;      // 干扰功率(kW)
    QString coverageRange;     // 覆盖范围(km)
};


#endif // STRUCTDATA_H