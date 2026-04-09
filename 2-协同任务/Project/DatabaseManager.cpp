#include "DatabaseManager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initDatabase(const QString& dbPath)
{
    if (dbPath.isEmpty()) {
        QString appPath = QCoreApplication::applicationDirPath();
        m_dbPath = appPath + "/mission_planning.db";
    } else {
        m_dbPath = dbPath;
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);

    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        emit databaseError(m_lastError);
        return false;
    }

    if (!createTables()) {
        return false;
    }

    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    QStringList createTableStatements = {
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

        R"(
        CREATE TABLE IF NOT EXISTS GroupInfos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            groupName TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        R"(
        CREATE TABLE IF NOT EXISTS EquipmentTypes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            groupId INTEGER NOT NULL,
            equipmentName TEXT NOT NULL,
            createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
            updatedAt TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (groupId) REFERENCES GroupInfos(id) ON DELETE CASCADE
        ))",

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

    for (const QString& sql : createTableStatements) {
        if (!query.exec(sql)) {
            m_lastError = query.lastError().text();
            emit databaseError(m_lastError);
            return false;
        }
    }

    return true;
}

bool DatabaseManager::clearAllTables()
{
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
    if (!m_database.transaction()) {
        m_lastError = m_database.lastError().text();
        return false;
    }

    if (!clearAllTables()) {
        m_database.rollback();
        return false;
    }

    if (!saveTaskInfos(taskList)) {
        m_database.rollback();
        return false;
    }

    if (!saveForceCalculations(forceList)) {
        m_database.rollback();
        return false;
    }

    if (!saveGroupInfos(groupList)) {
        m_database.rollback();
        return false;
    }

    if (!saveTaskAllocations(allocationList)) {
        m_database.rollback();
        return false;
    }

    if (!savePathPlanings(cruisePathList, searchPathList)) {
        m_database.rollback();
        return false;
    }

    if (!saveRadarTargetParams(radarList)) {
        m_database.rollback();
        return false;
    }

    if (!saveRadioTargetParams(radioList)) {
        m_database.rollback();
        return false;
    }

    if (!saveCommJammingParams(commList)) {
        m_database.rollback();
        return false;
    }

    if (!saveRcmJammingParams(rcmList)) {
        m_database.rollback();
        return false;
    }

    if (!m_database.commit()) {
        m_database.rollback();
        m_lastError = m_database.lastError().text();
        return false;
    }

    return true;
}

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

bool DatabaseManager::saveGroupInfos(const QList<GroupInfo>& groupList)
{
    QSqlQuery query(m_database);
    QSqlQuery equipQuery(m_database);
    QSqlQuery uavQuery(m_database);

    for (const GroupInfo& group : groupList) {
        query.prepare("INSERT INTO GroupInfos (groupName) VALUES (:groupName)");
        query.bindValue(":groupName", group.groupName);
        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }

        int groupId = query.lastInsertId().toInt();

        for (const EquipmentType& equip : group.equipmentList) {
            equipQuery.prepare("INSERT INTO EquipmentTypes (groupId, equipmentName) VALUES (:groupId, :equipmentName)");
            equipQuery.bindValue(":groupId", groupId);
            equipQuery.bindValue(":equipmentName", equip.equipmentName);
            if (!equipQuery.exec()) {
                m_lastError = equipQuery.lastError().text();
                return false;
            }

            int equipId = equipQuery.lastInsertId().toInt();

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

bool DatabaseManager::savePathPlanings(const QList<PathPlanning>& cruiseList, const QList<PathPlanning>& searchList)
{
    QSqlQuery query(m_database);
    QSqlQuery pointQuery(m_database);

    auto savePaths = [&](const QList<PathPlanning>& pathList, const QString& pathType) {
        for (const PathPlanning& path : pathList) {
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

    if (!savePaths(cruiseList, "cruise")) {
        return false;
    }

    if (!savePaths(searchList, "search")) {
        return false;
    }

    return true;
}

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

bool DatabaseManager::loadAllData(
    QList<TaskInfo>& taskList,
    QList<ForceCalculation>& forceList,
    QList<GroupInfo>& groupList,
    QList<TaskAllocation>& allocationList,
    QList<PathPlanning>& cruiseList,
    QList<PathPlanning>& searchList,
    QList<RadarTargetParam>& radarList,
    QList<RadioTargetParam>& radioList,
    QList<CommJammingParam>& commList,
    QList<RcmJammingParam>& rcmList
)
{
    loadTaskInfos(taskList);
    loadForceCalculations(forceList);
    loadGroupInfos(groupList);
    loadTaskAllocations(allocationList);
    loadPathPlanings(cruiseList, searchList);
    loadRadarTargetParams(radarList);
    loadRadioTargetParams(radioList);
    loadCommJammingParams(commList);
    loadRcmJammingParams(rcmList);

    return true;
}

bool DatabaseManager::loadTaskInfos(QList<TaskInfo>& taskList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT planName, coordinationName, taskName, taskType, targetType, taskTarget, startTime, endTime, allocatedUAVs FROM TaskInfos")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        TaskInfo task;
        task.planName = query.value(0).toString();
        task.coordinationName = query.value(1).toString();
        task.taskName = query.value(2).toString();
        task.taskType = query.value(3).toString();
        task.targetType = query.value(4).toString();
        task.taskTarget = query.value(5).toString();
        task.startTime = QTime::fromString(query.value(6).toString(), "HH:mm");
        task.endTime = QTime::fromString(query.value(7).toString(), "HH:mm");
        task.allocatedUAVs = query.value(8).toString();
        taskList.append(task);
    }
    return true;
}

bool DatabaseManager::loadForceCalculations(QList<ForceCalculation>& forceList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT planName, coordinationName, taskName, taskTarget, threatLevel, priority, calculatedCount, adjustedCount FROM ForceCalculations")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        ForceCalculation force;
        force.planName = query.value(0).toString();
        force.coordinationName = query.value(1).toString();
        force.taskName = query.value(2).toString();
        force.taskTarget = query.value(3).toString();
        force.threatLevel = query.value(4).toString();
        force.priority = query.value(5).toString();
        force.calculatedCount = query.value(6).toInt();
        force.adjustedCount = query.value(7).toInt();
        forceList.append(force);
    }
    return true;
}

bool DatabaseManager::loadGroupInfos(QList<GroupInfo>& groupList)
{
    QSqlQuery query(m_database);
    QSqlQuery equipQuery(m_database);
    QSqlQuery uavQuery(m_database);

    if (!query.exec("SELECT id, groupName FROM GroupInfos")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        GroupInfo group;
        int groupId = query.value(0).toInt();
        group.groupName = query.value(1).toString();

        equipQuery.exec(QString("SELECT id, equipmentName FROM EquipmentTypes WHERE groupId = %1").arg(groupId));
        while (equipQuery.next()) {
            EquipmentType equip;
            int equipId = equipQuery.value(0).toInt();
            equip.equipmentName = equipQuery.value(1).toString();

            uavQuery.exec(QString("SELECT uavName, status FROM UAVInfos WHERE equipmentId = %1").arg(equipId));
            while (uavQuery.next()) {
                UAVInfo uav;
                uav.uavName = uavQuery.value(0).toString();
                uav.status = uavQuery.value(1).toString();
                equip.uavList.append(uav);
            }
            group.equipmentList.append(equip);
        }
        groupList.append(group);
    }
    return true;
}

bool DatabaseManager::loadTaskAllocations(QList<TaskAllocation>& allocationList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT planName, coordinationName, taskName, targetType, taskTarget, threatLevel, allocatedUAVs, formation FROM TaskAllocations")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        TaskAllocation allocation;
        allocation.planName = query.value(0).toString();
        allocation.coordinationName = query.value(1).toString();
        allocation.taskName = query.value(2).toString();
        allocation.targetType = query.value(3).toString();
        allocation.taskTarget = query.value(4).toString();
        allocation.threatLevel = query.value(5).toString();
        allocation.allocatedUAVs = query.value(6).toString();
        allocation.formation = query.value(7).toString();
        allocationList.append(allocation);
    }
    return true;
}

bool DatabaseManager::loadPathPlanings(QList<PathPlanning>& cruiseList, QList<PathPlanning>& searchList)
{
    QSqlQuery query(m_database);
    QSqlQuery pointQuery(m_database);

    if (!query.exec("SELECT id, planName, coordinationName, uavName, relatedTask, pathPointCount, status, pathType FROM PathPlanings")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        PathPlanning path;
        int pathId = query.value(0).toInt();
        path.planName = query.value(1).toString();
        path.coordinationName = query.value(2).toString();
        path.uavName = query.value(3).toString();
        path.relatedTask = query.value(4).toString();
        path.pathPointCount = query.value(5).toInt();
        path.status = query.value(6).toString();
        QString pathType = query.value(7).toString();

        pointQuery.exec(QString("SELECT pointOrder, latitude, longitude, altitude FROM PathPoints WHERE pathId = %1 ORDER BY pointOrder").arg(pathId));
        while (pointQuery.next()) {
            PathPoint point;
            point.pointOrder = pointQuery.value(0).toInt();
            point.latitude = pointQuery.value(1).toDouble();
            point.longitude = pointQuery.value(2).toDouble();
            point.altitude = pointQuery.value(3).toDouble();
            path.pathPoints.append(point);
        }

        if (pathType == "cruise") {
            cruiseList.append(path);
        } else {
            searchList.append(path);
        }
    }
    return true;
}

bool DatabaseManager::loadRadarTargetParams(QList<RadarTargetParam>& radarList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT planName, coordinationName, targetId, frequencyRange, pulseWidthRange, repetitionPeriod, workingMode FROM RadarTargetParams")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        RadarTargetParam radar;
        radar.planName = query.value(0).toString();
        radar.coordinationName = query.value(1).toString();
        radar.targetId = query.value(2).toString();
        radar.frequencyRange = query.value(3).toString();
        radar.pulseWidthRange = query.value(4).toString();
        radar.repetitionPeriod = query.value(5).toString();
        radar.workingMode = query.value(6).toString();
        radarList.append(radar);
    }
    return true;
}

bool DatabaseManager::loadRadioTargetParams(QList<RadioTargetParam>& radioList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT planName, coordinationName, targetId, frequencyRange, modulationMode, signalBandwidth, transmitPower FROM RadioTargetParams")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        RadioTargetParam radio;
        radio.planName = query.value(0).toString();
        radio.coordinationName = query.value(1).toString();
        radio.targetId = query.value(2).toString();
        radio.frequencyRange = query.value(3).toString();
        radio.modulationMode = query.value(4).toString();
        radio.signalBandwidth = query.value(5).toString();
        radio.transmitPower = query.value(6).toString();
        radioList.append(radio);
    }
    return true;
}

bool DatabaseManager::loadCommJammingParams(QList<CommJammingParam>& commList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT planName, coordinationName, targetId, jammingFrequency, jammingMode, jammingPower, coverageRange FROM CommJammingParams")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        CommJammingParam comm;
        comm.planName = query.value(0).toString();
        comm.coordinationName = query.value(1).toString();
        comm.targetId = query.value(2).toString();
        comm.jammingFrequency = query.value(3).toString();
        comm.jammingMode = query.value(4).toString();
        comm.jammingPower = query.value(5).toString();
        comm.coverageRange = query.value(6).toString();
        commList.append(comm);
    }
    return true;
}

bool DatabaseManager::loadRcmJammingParams(QList<RcmJammingParam>& rcmList)
{
    QSqlQuery query(m_database);
    if (!query.exec("SELECT planName, coordinationName, targetId, jammingFrequency, jammingMode, jammingPower, coverageRange FROM RcmJammingParams")) {
        m_lastError = query.lastError().text();
        return false;
    }

    while (query.next()) {
        RcmJammingParam rcm;
        rcm.planName = query.value(0).toString();
        rcm.coordinationName = query.value(1).toString();
        rcm.targetId = query.value(2).toString();
        rcm.jammingFrequency = query.value(3).toString();
        rcm.jammingMode = query.value(4).toString();
        rcm.jammingPower = query.value(5).toString();
        rcm.coverageRange = query.value(6).toString();
        rcmList.append(rcm);
    }
    return true;
}
