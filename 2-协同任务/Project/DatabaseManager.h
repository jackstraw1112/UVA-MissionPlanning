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

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();
    ~DatabaseManager();

    bool initDatabase(const QString& dbPath = QString());
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

    QString getLastError() const { return m_lastError; }

signals:
    void databaseError(const QString& error);

private:
    DatabaseManager(QObject *parent = nullptr);
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    bool clearAllTables();

    bool saveTaskInfos(const QList<TaskInfo>& taskList);
    bool saveForceCalculations(const QList<ForceCalculation>& forceList);
    bool saveGroupInfos(const QList<GroupInfo>& groupList);
    bool saveTaskAllocations(const QList<TaskAllocation>& allocationList);
    bool savePathPlanings(const QList<PathPlanning>& cruiseList, const QList<PathPlanning>& searchList);
    bool saveRadarTargetParams(const QList<RadarTargetParam>& radarList);
    bool saveRadioTargetParams(const QList<RadioTargetParam>& radioList);
    bool saveCommJammingParams(const QList<CommJammingParam>& commList);
    bool saveRcmJammingParams(const QList<RcmJammingParam>& rcmList);

    bool loadTaskInfos(QList<TaskInfo>& taskList);
    bool loadForceCalculations(QList<ForceCalculation>& forceList);
    bool loadGroupInfos(QList<GroupInfo>& groupList);
    bool loadTaskAllocations(QList<TaskAllocation>& allocationList);
    bool loadPathPlanings(QList<PathPlanning>& cruiseList, QList<PathPlanning>& searchList);
    bool loadRadarTargetParams(QList<RadarTargetParam>& radarList);
    bool loadRadioTargetParams(QList<RadioTargetParam>& radioList);
    bool loadCommJammingParams(QList<CommJammingParam>& commList);
    bool loadRcmJammingParams(QList<RcmJammingParam>& rcmList);

    QSqlDatabase m_database;
    QString m_lastError;
    QString m_dbPath;
};

#endif // DATABASEMANAGER_H
