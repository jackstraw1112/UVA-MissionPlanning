#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QList>
#include "StructData.h"

/**
 * @brief 数据库管理器类
 * @details 负责所有数据的持久化存储，支持任务、兵力、编组、路径规划、目标参数等数据的保存和加载
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     * @return 数据库管理器引用
     */
    static DatabaseManager& instance();

    /**
     * @brief 析构函数
     */
    ~DatabaseManager();

    /**
     * @brief 初始化数据库
     * @param dbPath 数据库路径，默认为空使用默认路径
     * @return 初始化是否成功
     */
    bool initDatabase(const QString& dbPath = QString());

    /**
     * @brief 保存所有数据
     * @param taskList 任务信息列表
     * @param forceList 兵力需求列表
     * @param groupList 编组信息列表
     * @param allocationList 任务分配列表
     * @param cruisePathList 巡航路径列表
     * @param searchPathList 搜索路径列表
     * @param radarList 雷达目标列表
     * @param radioList 无线电目标列表
     * @param commList 通信干扰列表
     * @param rcmList 雷达干扰列表
     * @return 保存是否成功
     */
    bool saveAllData(
        const QList<TaskInfo>& taskList,
        const QList<ForceCalculation>& forceList,
        const QList<GroupInfo>& groupList,
        const QList<TaskAllocation>& allocationList,
        const QList<PathPlanning>& cruisePathList,
        const QList<PathPlanning>& searchPathList,
        const QList<RadarTargetParam>& radarList,
        const QList<RadioTargetParam>& radioList,
        const QList<CommJammingParam>& commList,
        const QList<RcmJammingParam>& rcmList
    );

    /**
     * @brief 加载所有数据
     * @param taskList 任务信息列表（输出）
     * @param forceList 兵力需求列表（输出）
     * @param groupList 编组信息列表（输出）
     * @param allocationList 任务分配列表（输出）
     * @param cruisePathList 巡航路径列表（输出）
     * @param searchPathList 搜索路径列表（输出）
     * @param radarList 雷达目标列表（输出）
     * @param radioList 无线电目标列表（输出）
     * @param commList 通信干扰列表（输出）
     * @param rcmList 雷达干扰列表（输出）
     * @return 加载是否成功
     */
    bool loadAllData(
        QList<TaskInfo>& taskList,
        QList<ForceCalculation>& forceList,
        QList<GroupInfo>& groupList,
        QList<TaskAllocation>& allocationList,
        QList<PathPlanning>& cruisePathList,
        QList<PathPlanning>& searchPathList,
        QList<RadarTargetParam>& radarList,
        QList<RadioTargetParam>& radioList,
        QList<CommJammingParam>& commList,
        QList<RcmJammingParam>& rcmList
    );

    /**
     * @brief 获取最后错误信息
     * @return 错误信息字符串
     */
    QString getLastError() const { return m_lastError; }

signals:
    /**
     * @brief 数据库错误信号
     * @param error 错误信息
     */
    void databaseError(const QString& error);

private:
    /**
     * @brief 构造函数（私有，单例模式）
     * @param parent 父对象
     */
    DatabaseManager(QObject *parent = nullptr);

    // 禁用拷贝构造和赋值操作（单例模式）
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    /**
     * @brief 创建数据库表
     * @return 创建是否成功
     */
    bool createTables();

    /**
     * @brief 清空所有数据表
     * @return 清空是否成功
     */
    bool clearAllTables();

    // 保存函数
    bool saveTaskInfos(const QList<TaskInfo>& taskList);                      // 保存任务信息
    bool saveForceCalculations(const QList<ForceCalculation>& forceList);      // 保存兵力需求
    bool saveGroupInfos(const QList<GroupInfo>& groupList);                     // 保存编组信息
    bool saveTaskAllocations(const QList<TaskAllocation>& allocationList);      // 保存任务分配
    bool savePathPlanings(const QList<PathPlanning>& cruiseList, const QList<PathPlanning>& searchList);  // 保存路径规划
    bool saveRadarTargetParams(const QList<RadarTargetParam>& radarList);      // 保存雷达目标参数
    bool saveRadioTargetParams(const QList<RadioTargetParam>& radioList);      // 保存无线电目标参数
    bool saveCommJammingParams(const QList<CommJammingParam>& commList);        // 保存通信干扰参数
    bool saveRcmJammingParams(const QList<RcmJammingParam>& rcmList);           // 保存雷达干扰参数

    // 加载函数
    bool loadTaskInfos(QList<TaskInfo>& taskList);                      // 加载任务信息
    bool loadForceCalculations(QList<ForceCalculation>& forceList);      // 加载兵力需求
    bool loadGroupInfos(QList<GroupInfo>& groupList);                     // 加载编组信息
    bool loadTaskAllocations(QList<TaskAllocation>& allocationList);      // 加载任务分配
    bool loadPathPlanings(QList<PathPlanning>& cruiseList, QList<PathPlanning>& searchList);  // 加载路径规划
    bool loadRadarTargetParams(QList<RadarTargetParam>& radarList);      // 加载雷达目标参数
    bool loadRadioTargetParams(QList<RadioTargetParam>& radioList);      // 加载无线电目标参数
    bool loadCommJammingParams(QList<CommJammingParam>& commList);        // 加载通信干扰参数
    bool loadRcmJammingParams(QList<RcmJammingParam>& rcmList);           // 加载雷达干扰参数

    QSqlDatabase m_database;  // 数据库连接
    QString m_lastError;      // 最后错误信息
    QString m_dbPath;         // 数据库路径
};

#endif // DATABASEMANAGER_H
