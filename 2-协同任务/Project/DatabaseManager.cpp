#include "DatabaseManager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

/**
 * @brief 构造函数（私有，单例模式）
 * @param parent 父对象
 */
DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

/**
 * @brief 析构函数
 * @details 关闭数据库连接
 */
DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

/**
 * @brief 获取单例实例
 * @return 数据库管理器引用
 */
DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

/**
 * @brief 初始化数据库
 * @param dbPath 数据库路径，默认为空使用应用目录下的mission_planning.db
 * @return 初始化是否成功
 */
bool DatabaseManager::initDatabase(const QString& dbPath)
{
    // 设置数据库路径
    if (dbPath.isEmpty()) {
        QString appPath = QCoreApplication::applicationDirPath();
        m_dbPath = appPath + "/mission_planning.db";
    } else {
        m_dbPath = dbPath;
    }

    // 添加SQLite数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);

    // 打开数据库
    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        emit databaseError(m_lastError);
        return false;
    }

    // 创建数据表
    if (!createTables()) {
        return false;
    }

    return true;
}

/**
 * @brief 创建数据库表
 * @return 创建是否成功
 * @details 创建所有必要的数据表，包括任务信息、兵力需求、编组信息、路径规划、目标参数等
 */
bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    // 定义所有建表SQL语句
    QStringList createTableStatements = {
        // 任务信息表
        R"(
        CREATE TABLE IF NOT EXISTS TaskInfos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            planName TEXT NOT NULL,
            coordinationName TEXT NOT NULL,
            taskName TEXT NOT NULL,
            taskType TEXT NOT NULL,
            targetType TEXT NOT NULL,
            taskTarget TEXT NOT NULL,
            startTime TEXT NOT NULL,
            endTime TEXT NOT NULL,
            allocatedUAVs TEXT,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 兵力需求计算表
        R"(
        CREATE TABLE IF NOT EXISTS ForceCalculations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            planName TEXT NOT NULL,
            coordinationName TEXT NOT NULL,
            taskName TEXT NOT NULL,
            taskTarget TEXT NOT NULL,
            threatLevel TEXT NOT NULL,
            priority TEXT NOT NULL,
            calculatedCount INTEGER NOT NULL,
            adjustedCount INTEGER NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 编组信息表
        R"(
        CREATE TABLE IF NOT EXISTS GroupInfos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            groupName TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 装备类型表
        R"(
        CREATE TABLE IF NOT EXISTS EquipmentTypes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            groupId INTEGER NOT NULL,
            equipmentName TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (groupId) REFERENCES GroupInfos(id) ON DELETE CASCADE
        ))",

        // 无人机信息表
        R"(
        CREATE TABLE IF NOT EXISTS UAVInfos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            equipmentId INTEGER NOT NULL,
            uavName TEXT NOT NULL,
            status TEXT NOT NULL DEFAULT '待命',
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (equipmentId) REFERENCES EquipmentTypes(id) ON DELETE CASCADE
        ))",

        // 雷达目标参数表
        R"(
        CREATE TABLE IF NOT EXISTS RadarTargetParams (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            planName TEXT NOT NULL,
            coordinationName TEXT NOT NULL,
            targetId TEXT NOT NULL,
            frequencyRange TEXT NOT NULL,
            pulseWidthRange TEXT NOT NULL,
            repetitionPeriod TEXT NOT NULL,
            workingMode TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 无线电目标参数表
        R"(
        CREATE TABLE IF NOT EXISTS RadioTargetParams (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            planName TEXT NOT NULL,
            coordinationName TEXT NOT NULL,
            targetId TEXT NOT NULL,
            frequencyRange TEXT NOT NULL,
            modulationMode TEXT NOT NULL,
            signalBandwidth TEXT NOT NULL,
            transmitPower TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 通信干扰参数表
        R"(
        CREATE TABLE IF NOT EXISTS CommJammingParams (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            planName TEXT NOT NULL,
            coordinationName TEXT NOT NULL,
            targetId TEXT NOT NULL,
            jammingFrequency TEXT NOT NULL,
            jammingMode TEXT NOT NULL,
            jammingPower TEXT NOT NULL,
            coverageRange TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 雷达干扰参数表
        R"(
        CREATE TABLE IF NOT EXISTS RcmJammingParams (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            planName TEXT NOT NULL,
            coordinationName TEXT NOT NULL,
            targetId TEXT NOT NULL,
            jammingFrequency TEXT NOT NULL,
            jammingMode TEXT NOT NULL,
            jammingPower TEXT NOT NULL,
            coverageRange TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 任务分配表
        R"(
        CREATE TABLE IF NOT EXISTS TaskAllocations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            planName TEXT NOT NULL,
            coordinationName TEXT NOT NULL,
            taskName TEXT NOT NULL,
            targetType TEXT NOT NULL,
            taskTarget TEXT NOT NULL,
            threatLevel TEXT NOT NULL,
            allocatedUAVs TEXT NOT NULL,
            formation TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 路径规划表
        R"(
        CREATE TABLE IF NOT EXISTS PathPlanings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            planName TEXT NOT NULL,
            coordinationName TEXT NOT NULL,
            uavName TEXT NOT NULL,
            relatedTask TEXT NOT NULL,
            pathPointCount INTEGER NOT NULL,
            status TEXT NOT NULL DEFAULT '待生成',
            pathType TEXT NOT NULL DEFAULT 'cruise',
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // 路径点表
        R"(
        CREATE TABLE IF NOT EXISTS PathPoints (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            pathId INTEGER NOT NULL,
            pointOrder INTEGER NOT NULL,
            latitude REAL NOT NULL,
            longitude REAL NOT NULL,
            altitude REAL NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (pathId) REFERENCES PathPlanings(id) ON DELETE CASCADE
        ))"
    };

    // 执行建表语句
    for (const QString& sql : createTableStatements) {
        if (!query.exec(sql)) {
            m_lastError = query.lastError().text();
            emit databaseError(m_lastError);
            return false;
        }
    }

    return true;
}

/**
 * @brief 清空所有数据表
 * @return 清空是否成功
 * @details 按照依赖关系顺序删除所有表数据
 */
bool DatabaseManager::clearAllTables()
{
    // 按照依赖关系顺序定义要清空的表
    QStringList tables = {
        "PathPoints", "PathPlanings", "TaskAllocations",
        "RcmJammingParams", "CommJammingParams", "RadioTargetParams",
        "RadarTargetParams", "UAVInfos", "EquipmentTypes",
        "GroupInfos", "ForceCalculations", "TaskInfos"
    };

    QSqlQuery query(m_database);
    for (const QString& table : tables) {
        if (!query.exec(QString("DELETE FROM %1").arg(table))) {
            m_lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

/**
 * @brief 保存所有数据
 * @details 开启事务，清空旧数据，逐个保存各类数据，最后提交事务
 */
bool DatabaseManager::saveAllData(
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
)
{
    // 开启事务
    if (!m_database.transaction()) {
        m_lastError = m_database.lastError().text();
        return false;
    }

    // 清空所有表
    if (!clearAllTables()) {
        m_database.rollback();
        return false;
    }

    // 保存任务信息
    if (!saveTaskInfos(taskList)) {
        m_database.rollback();
        return false;
    }

    // 保存兵力需求
    if (!saveForceCalculations(forceList)) {
        m_database.rollback();
        return false;
    }

    // 保存编组信息
    if (!saveGroupInfos(groupList)) {
        m_database.rollback();
        return false;
    }

    // 保存任务分配
    if (!saveTaskAllocations(allocationList)) {
        m_database.rollback();
        return false;
    }

    // 保存路径规划
    if (!savePathPlanings(cruisePathList, searchPathList)) {
        m_database.rollback();
        return false;
    }

    // 保存雷达目标参数
    if (!saveRadarTargetParams(radarList)) {
        m_database.rollback();
        return false;
    }

    // 保存无线电目标参数
    if (!saveRadioTargetParams(radioList)) {
        m_database.rollback();
        return false;
    }

    // 保存通信干扰参数
    if (!saveCommJammingParams(commList)) {
        m_database.rollback();
        return false;
    }

    // 保存雷达干扰参数
    if (!saveRcmJammingParams(rcmList)) {
        m_database.rollback();
        return false;
    }

    // 提交事务
    if (!m_database.commit()) {
        m_database.rollback();
        m_lastError = m_database.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief 保存任务信息列表
 * @param taskList 任务信息列表
 * @return 保存是否成功
 */
bool DatabaseManager::saveTaskInfos(const QList<TaskInfo>& taskList)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO TaskInfos (planName, coordinationName, taskName, taskType, targetType, taskTarget, startTime, endTime, allocatedUAVs)
        VALUES (:planName, :coordinationName, :taskName, :taskType, :targetType, :taskTarget, :startTime, :endTime, :allocatedUAVs)
    )");

    for (const TaskInfo& task : taskList) {
        query.bindValue(":planName", task.planName);
        query.bindValue(":coordinationName", task.coordinationName);
        query.bindValue(":taskName", task.taskName);
        query.bindValue(":taskType", task.taskType);
        query.bindValue(":targetType", task.targetType);
        query.bindValue(":taskTarget", task.taskTarget);
        query.bindValue(":startTime", task.startTime.toString("HH:mm"));
        query.bindValue(":endTime", task.endTime.toString("HH:mm"));
        query.bindValue(":allocatedUAVs", task.allocatedUAVs);

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

/**
 * @brief 保存兵力需求列表
 * @param forceList 兵力需求列表
 * @return 保存是否成功
 */
bool DatabaseManager::saveForceCalculations(const QList<ForceCalculation>& forceList)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO ForceCalculations (planName, coordinationName, taskName, taskTarget, threatLevel, priority, calculatedCount, adjustedCount)
        VALUES (:planName, :coordinationName, :taskName, :taskTarget, :threatLevel, :priority, :calculatedCount, :adjustedCount)
    )");

    for (const ForceCalculation& force : forceList) {
        query.bindValue(":planName", force.planName);
        query.bindValue(":coordinationName", force.coordinationName);
        query.bindValue(":taskName", force.taskName);
        query.bindValue(":taskTarget", force.taskTarget);
        query.bindValue(":threatLevel", force.threatLevel);
        query.bindValue(":priority", force.priority);
        query.bindValue(":calculatedCount", force.calculatedCount);
        query.bindValue(":adjustedCount", force.adjustedCount);

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

/**
 * @brief 保存编组信息列表
 * @param groupList 编组信息列表
 * @return 保存是否成功
 * @details 包含嵌套的装备类型和无人机信息
 */
bool DatabaseManager::saveGroupInfos(const QList<GroupInfo>& groupList)
{
    QSqlQuery query(m_database);
    QSqlQuery equipQuery(m_database);
    QSqlQuery uavQuery(m_database);

    for (const GroupInfo& group : groupList) {
        // 保存编组信息
        query.prepare("INSERT INTO GroupInfos (groupName) VALUES (:groupName)");
        query.bindValue(":groupName", group.groupName);
        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }

        int groupId = query.lastInsertId().toInt();

        // 保存装备类型信息
        for (const EquipmentType& equip : group.equipmentList) {
            equipQuery.prepare("INSERT INTO EquipmentTypes (groupId, equipmentName) VALUES (:groupId, :equipmentName)");
            equipQuery.bindValue(":groupId", groupId);
            equipQuery.bindValue(":equipmentName", equip.equipmentName);
            if (!equipQuery.exec()) {
                m_lastError = equipQuery.lastError().text();
                return false;
            }

            int equipId = equipQuery.lastInsertId().toInt();

            // 保存无人机信息
            for (const UAVInfo& uav : equip.uavList) {
                uavQuery.prepare("INSERT INTO UAVInfos (equipmentId, uavName, status) VALUES (:equipmentId, :uavName, :status)");
                uavQuery.bindValue(":equipmentId", equipId);
                uavQuery.bindValue(":uavName", uav.uavName);
                uavQuery.bindValue(":status", uav.status);
                if (!uavQuery.exec()) {
                    m_lastError = uavQuery.lastError().text();
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * @brief 保存任务分配列表
 * @param allocationList 任务分配列表
 * @return 保存是否成功
 */
bool DatabaseManager::saveTaskAllocations(const QList<TaskAllocation>& allocationList)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO TaskAllocations (planName, coordinationName, taskName, targetType, taskTarget, threatLevel, allocatedUAVs, formation)
        VALUES (:planName, :coordinationName, :taskName, :targetType, :taskTarget, :threatLevel, :allocatedUAVs, :formation)
    )");

    for (const TaskAllocation& allocation : allocationList) {
        query.bindValue(":planName", allocation.planName);
        query.bindValue(":coordinationName", allocation.coordinationName);
        query.bindValue(":taskName", allocation.taskName);
        query.bindValue(":targetType", allocation.targetType);
        query.bindValue(":taskTarget", allocation.taskTarget);
        query.bindValue(":threatLevel", allocation.threatLevel);
        query.bindValue(":allocatedUAVs", allocation.allocatedUAVs);
        query.bindValue(":formation", allocation.formation);

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

/**
 * @brief 保存路径规划列表
 * @param cruiseList 巡航路径列表
 * @param searchList 搜索路径列表
 * @return 保存是否成功
 * @details 同时保存路径点和路径基本信息
 */
bool DatabaseManager::savePathPlanings(const QList<PathPlanning>& cruiseList, const QList<PathPlanning>& searchList)
{
    QSqlQuery query(m_database);
    QSqlQuery pointQuery(m_database);

    // 保存路径的通用函数
    auto savePaths = [&](const QList<PathPlanning>& pathList, const QString& pathType) {
        for (const PathPlanning& path : pathList) {
            // 保存路径基本信息
            query.prepare(R"(
                INSERT INTO PathPlanings (planName, coordinationName, uavName, relatedTask, pathPointCount, status, pathType)
                VALUES (:planName, :coordinationName, :uavName, :relatedTask, :pathPointCount, :status, :pathType)
            )");
            query.bindValue(":planName", path.planName);
            query.bindValue(":coordinationName", path.coordinationName);
            query.bindValue(":uavName", path.uavName);
            query.bindValue(":relatedTask", path.relatedTask);
            query.bindValue(":pathPointCount", path.pathPointCount);
            query.bindValue(":status", path.status);
            query.bindValue(":pathType", pathType);

            if (!query.exec()) {
                m_lastError = query.lastError().text();
                return false;
            }

            int pathId = query.lastInsertId().toInt();

            // 保存路径点信息
            for (const PathPoint& point : path.pathPoints) {
                pointQuery.prepare(R"(
                    INSERT INTO PathPoints (pathId, pointOrder, latitude, longitude, altitude)
                    VALUES (:pathId, :pointOrder, :latitude, :longitude, :altitude)
                )");
                pointQuery.bindValue(":pathId", pathId);
                pointQuery.bindValue(":pointOrder", point.pointOrder);
                pointQuery.bindValue(":latitude", point.latitude);
                pointQuery.bindValue(":longitude", point.longitude);
                pointQuery.bindValue(":altitude", point.altitude);

                if (!pointQuery.exec()) {
                    m_lastError = pointQuery.lastError().text();
                    return false;
                }
            }
        }
        return true;
    };

    // 保存巡航路径
    if (!savePaths(cruiseList, "cruise")) {
        return false;
    }

    // 保存搜索路径
    if (!savePaths(searchList, "search")) {
        return false;
    }

    return true;
}

/**
 * @brief 保存雷达目标参数列表
 * @param radarList 雷达目标参数列表
 * @return 保存是否成功
 */
bool DatabaseManager::saveRadarTargetParams(const QList<RadarTargetParam>& radarList)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO RadarTargetParams (planName, coordinationName, targetId, frequencyRange, pulseWidthRange, repetitionPeriod, workingMode)
        VALUES (:planName, :coordinationName, :targetId, :frequencyRange, :pulseWidthRange, :repetitionPeriod, :workingMode)
    )");

    for (const RadarTargetParam& radar : radarList) {
        query.bindValue(":planName", radar.planName);
        query.bindValue(":coordinationName", radar.coordinationName);
        query.bindValue(":targetId", radar.targetId);
        query.bindValue(":frequencyRange", radar.frequencyRange);
        query.bindValue(":pulseWidthRange", radar.pulseWidthRange);
        query.bindValue(":repetitionPeriod", radar.repetitionPeriod);
        query.bindValue(":workingMode", radar.workingMode);

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

/**
 * @brief 保存无线电目标参数列表
 * @param radioList 无线电目标参数列表
 * @return 保存是否成功
 */
bool DatabaseManager::saveRadioTargetParams(const QList<RadioTargetParam>& radioList)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO RadioTargetParams (planName, coordinationName, targetId, frequencyRange, modulationMode, signalBandwidth, transmitPower)
        VALUES (:planName, :coordinationName, :targetId, :frequencyRange, :modulationMode, :signalBandwidth, :transmitPower)
    )");

    for (const RadioTargetParam& radio : radioList) {
        query.bindValue(":planName", radio.planName);
        query.bindValue(":coordinationName", radio.coordinationName);
        query.bindValue(":targetId", radio.targetId);
        query.bindValue(":frequencyRange", radio.frequencyRange);
        query.bindValue(":modulationMode", radio.modulationMode);
        query.bindValue(":signalBandwidth", radio.signalBandwidth);
        query.bindValue(":transmitPower", radio.transmitPower);

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

/**
 * @brief 保存通信干扰参数列表
 * @param commList 通信干扰参数列表
 * @return 保存是否成功
 */
bool DatabaseManager::saveCommJammingParams(const QList<CommJammingParam>& commList)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO CommJammingParams (planName, coordinationName, targetId, jammingFrequency, jammingMode, jammingPower, coverageRange)
        VALUES (:planName, :coordinationName, :targetId, :jammingFrequency, :jammingMode, :jammingPower, :coverageRange)
    )");

    for (const CommJammingParam& comm : commList) {
        query.bindValue(":planName", comm.planName);
        query.bindValue(":coordinationName", comm.coordinationName);
        query.bindValue(":targetId", comm.targetId);
        query.bindValue(":jammingFrequency", comm.jammingFrequency);
        query.bindValue(":jammingMode", comm.jammingMode);
        query.bindValue(":jammingPower", comm.jammingPower);
        query.bindValue(":coverageRange", comm.coverageRange);

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

/**
 * @brief 保存雷达干扰参数列表
 * @param rcmList 雷达干扰参数列表
 * @return 保存是否成功
 */
bool DatabaseManager::saveRcmJammingParams(const QList<RcmJammingParam>& rcmList)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO RcmJammingParams (planName, coordinationName, targetId, jammingFrequency, jammingMode, jammingPower, coverageRange)
        VALUES (:planName, :coordinationName, :targetId, :jammingFrequency, :jammingMode, :jammingPower, :coverageRange)
    )");

    for (const RcmJammingParam& rcm : rcmList) {
        query.bindValue(":planName", rcm.planName);
        query.bindValue(":coordinationName", rcm.coordinationName);
        query.bindValue(":targetId", rcm.targetId);
        query.bindValue(":jammingFrequency", rcm.jammingFrequency);
        query.bindValue(":jammingMode", rcm.jammingMode);
        query.bindValue(":jammingPower", rcm.jammingPower);
        query.bindValue(":coverageRange", rcm.coverageRange);

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

/**
 * @brief 加载所有数据
 * @details 从数据库加载所有类型的数据
 */
bool DatabaseManager::loadAllData(
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
)
{
    // 清空输出列表
    taskList.clear();
    forceList.clear();
    groupList.clear();
    allocationList.clear();
    cruisePathList.clear();
    searchPathList.clear();
    radarList.clear();
    radioList.clear();
    commList.clear();
    rcmList.clear();

    // 加载各类数据
    if (!loadTaskInfos(taskList)) return false;
    if (!loadForceCalculations(forceList)) return false;
    if (!loadGroupInfos(groupList)) return false;
    if (!loadTaskAllocations(allocationList)) return false;
    if (!loadPathPlanings(cruisePathList, searchPathList)) return false;
    if (!loadRadarTargetParams(radarList)) return false;
    if (!loadRadioTargetParams(radioList)) return false;
    if (!loadCommJammingParams(commList)) return false;
    if (!loadRcmJammingParams(rcmList)) return false;

    return true;
}

/**
 * @brief 加载任务信息列表
 * @param taskList 任务信息列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadTaskInfos(QList<TaskInfo>& taskList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM TaskInfos ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        TaskInfo task;
        task.planName = query.value("planName").toString();
        task.coordinationName = query.value("coordinationName").toString();
        task.taskName = query.value("taskName").toString();
        task.taskType = query.value("taskType").toString();
        task.targetType = query.value("targetType").toString();
        task.taskTarget = query.value("taskTarget").toString();
        task.startTime = QTime::fromString(query.value("startTime").toString(), "HH:mm");
        task.endTime = QTime::fromString(query.value("endTime").toString(), "HH:mm");
        task.allocatedUAVs = query.value("allocatedUAVs").toString();
        taskList.append(task);
    }
    return true;
}

/**
 * @brief 加载兵力需求列表
 * @param forceList 兵力需求列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadForceCalculations(QList<ForceCalculation>& forceList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM ForceCalculations ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        ForceCalculation force;
        force.planName = query.value("planName").toString();
        force.coordinationName = query.value("coordinationName").toString();
        force.taskName = query.value("taskName").toString();
        force.taskTarget = query.value("taskTarget").toString();
        force.threatLevel = query.value("threatLevel").toString();
        force.priority = query.value("priority").toString();
        force.calculatedCount = query.value("calculatedCount").toInt();
        force.adjustedCount = query.value("adjustedCount").toInt();
        forceList.append(force);
    }
    return true;
}

/**
 * @brief 加载编组信息列表
 * @param groupList 编组信息列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadGroupInfos(QList<GroupInfo>& groupList)
{
    QSqlQuery query(m_database);
    QSqlQuery equipQuery(m_database);
    QSqlQuery uavQuery(m_database);

    // 加载编组信息
    if (!query.exec("SELECT * FROM GroupInfos ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        GroupInfo group;
        group.groupName = query.value("groupName").toString();
        int groupId = query.value("id").toInt();

        // 加载装备类型
        equipQuery.prepare("SELECT * FROM EquipmentTypes WHERE groupId = :groupId ORDER BY id");
        equipQuery.bindValue(":groupId", groupId);
        if (!equipQuery.exec()) {
            m_lastError = equipQuery.lastError().text();
            return false;
        }

        while (equipQuery.next()) {
            EquipmentType equip;
            equip.equipmentName = equipQuery.value("equipmentName").toString();
            int equipId = equipQuery.value("id").toInt();

            // 加载无人机信息
            uavQuery.prepare("SELECT * FROM UAVInfos WHERE equipmentId = :equipmentId ORDER BY id");
            uavQuery.bindValue(":equipmentId", equipId);
            if (!uavQuery.exec()) {
                m_lastError = uavQuery.lastError().text();
                return false;
            }

            while (uavQuery.next()) {
                UAVInfo uav;
                uav.uavName = uavQuery.value("uavName").toString();
                uav.status = uavQuery.value("status").toString();
                equip.uavList.append(uav);
            }

            group.equipmentList.append(equip);
        }

        groupList.append(group);
    }
    return true;
}

/**
 * @brief 加载任务分配列表
 * @param allocationList 任务分配列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadTaskAllocations(QList<TaskAllocation>& allocationList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM TaskAllocations ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        TaskAllocation allocation;
        allocation.planName = query.value("planName").toString();
        allocation.coordinationName = query.value("coordinationName").toString();
        allocation.taskName = query.value("taskName").toString();
        allocation.targetType = query.value("targetType").toString();
        allocation.taskTarget = query.value("taskTarget").toString();
        allocation.threatLevel = query.value("threatLevel").toString();
        allocation.allocatedUAVs = query.value("allocatedUAVs").toString();
        allocation.formation = query.value("formation").toString();
        allocationList.append(allocation);
    }
    return true;
}

/**
 * @brief 加载路径规划列表
 * @param cruiseList 巡航路径列表（输出）
 * @param searchList 搜索路径列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadPathPlanings(QList<PathPlanning>& cruiseList, QList<PathPlanning>& searchList)
{
    QSqlQuery query(m_database);
    QSqlQuery pointQuery(m_database);

    // 加载路径规划信息
    if (!query.exec("SELECT * FROM PathPlanings ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        PathPlanning path;
        path.planName = query.value("planName").toString();
        path.coordinationName = query.value("coordinationName").toString();
        path.uavName = query.value("uavName").toString();
        path.relatedTask = query.value("relatedTask").toString();
        path.pathPointCount = query.value("pathPointCount").toInt();
        path.status = query.value("status").toString();
        QString pathType = query.value("pathType").toString();
        int pathId = query.value("id").toInt();

        // 加载路径点
        pointQuery.prepare("SELECT * FROM PathPoints WHERE pathId = :pathId ORDER BY pointOrder");
        pointQuery.bindValue(":pathId", pathId);
        if (!pointQuery.exec()) {
            m_lastError = pointQuery.lastError().text();
            return false;
        }

        while (pointQuery.next()) {
            PathPoint point;
            point.pointOrder = pointQuery.value("pointOrder").toInt();
            point.latitude = pointQuery.value("latitude").toDouble();
            point.longitude = pointQuery.value("longitude").toDouble();
            point.altitude = pointQuery.value("altitude").toDouble();
            path.pathPoints.append(point);
        }

        // 根据路径类型添加到对应列表
        if (pathType == "cruise") {
            cruiseList.append(path);
        } else {
            searchList.append(path);
        }
    }
    return true;
}

/**
 * @brief 加载雷达目标参数列表
 * @param radarList 雷达目标参数列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadRadarTargetParams(QList<RadarTargetParam>& radarList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM RadarTargetParams ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        RadarTargetParam radar;
        radar.planName = query.value("planName").toString();
        radar.coordinationName = query.value("coordinationName").toString();
        radar.targetId = query.value("targetId").toString();
        radar.frequencyRange = query.value("frequencyRange").toString();
        radar.pulseWidthRange = query.value("pulseWidthRange").toString();
        radar.repetitionPeriod = query.value("repetitionPeriod").toString();
        radar.workingMode = query.value("workingMode").toString();
        radarList.append(radar);
    }
    return true;
}

/**
 * @brief 加载无线电目标参数列表
 * @param radioList 无线电目标参数列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadRadioTargetParams(QList<RadioTargetParam>& radioList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM RadioTargetParams ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        RadioTargetParam radio;
        radio.planName = query.value("planName").toString();
        radio.coordinationName = query.value("coordinationName").toString();
        radio.targetId = query.value("targetId").toString();
        radio.frequencyRange = query.value("frequencyRange").toString();
        radio.modulationMode = query.value("modulationMode").toString();
        radio.signalBandwidth = query.value("signalBandwidth").toString();
        radio.transmitPower = query.value("transmitPower").toString();
        radioList.append(radio);
    }
    return true;
}

/**
 * @brief 加载通信干扰参数列表
 * @param commList 通信干扰参数列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadCommJammingParams(QList<CommJammingParam>& commList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM CommJammingParams ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        CommJammingParam comm;
        comm.planName = query.value("planName").toString();
        comm.coordinationName = query.value("coordinationName").toString();
        comm.targetId = query.value("targetId").toString();
        comm.jammingFrequency = query.value("jammingFrequency").toString();
        comm.jammingMode = query.value("jammingMode").toString();
        comm.jammingPower = query.value("jammingPower").toString();
        comm.coverageRange = query.value("coverageRange").toString();
        commList.append(comm);
    }
    return true;
}

/**
 * @brief 加载雷达干扰参数列表
 * @param rcmList 雷达干扰参数列表（输出）
 * @return 加载是否成功
 */
bool DatabaseManager::loadRcmJammingParams(QList<RcmJammingParam>& rcmList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM RcmJammingParams ORDER BY id")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        RcmJammingParam rcm;
        rcm.planName = query.value("planName").toString();
        rcm.coordinationName = query.value("coordinationName").toString();
        rcm.targetId = query.value("targetId").toString();
        rcm.jammingFrequency = query.value("jammingFrequency").toString();
        rcm.jammingMode = query.value("jammingMode").toString();
        rcm.jammingPower = query.value("jammingPower").toString();
        rcm.coverageRange = query.value("coverageRange").toString();
        rcmList.append(rcm);
    }
    return true;
}
