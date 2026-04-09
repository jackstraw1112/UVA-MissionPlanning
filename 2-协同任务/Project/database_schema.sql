-- 无人机任务规划数据库
-- 数据库版本
DROP TABLE IF EXISTS PathPoints;
DROP TABLE IF EXISTS PathPlanings;
DROP TABLE IF EXISTS TaskAllocations;
DROP TABLE IF EXISTS RcmJammingParams;
DROP TABLE IF EXISTS CommJammingParams;
DROP TABLE IF EXISTS RadioTargetParams;
DROP TABLE IF EXISTS RadarTargetParams;
DROP TABLE IF EXISTS UAVInfos;
DROP TABLE IF EXISTS EquipmentTypes;
DROP TABLE IF EXISTS GroupInfos;
DROP TABLE IF EXISTS ForceCalculations;
DROP TABLE IF EXISTS TaskInfos;

-- 任务信息表
CREATE TABLE TaskInfos (
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
);

-- 兵力需求计算表
CREATE TABLE ForceCalculations (
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
);

-- 编组信息表
CREATE TABLE GroupInfos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    groupName TEXT NOT NULL,
    createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
    updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
);

-- 装备型号表
CREATE TABLE EquipmentTypes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    groupId INTEGER NOT NULL,
    equipmentName TEXT NOT NULL,
    createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
    updatedAt TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (groupId) REFERENCES GroupInfos(id) ON DELETE CASCADE
);

-- 无人机信息表
CREATE TABLE UAVInfos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    equipmentId INTEGER NOT NULL,
    uavName TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT '待命',
    createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
    updatedAt TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (equipmentId) REFERENCES EquipmentTypes(id) ON DELETE CASCADE
);

-- 雷达目标参数表
CREATE TABLE RadarTargetParams (
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
);

-- 电台目标参数表
CREATE TABLE RadioTargetParams (
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
);

-- 通信对抗参数表
CREATE TABLE CommJammingParams (
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
);

-- 雷达对抗参数表
CREATE TABLE RcmJammingParams (
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
);

-- 任务分配表
CREATE TABLE TaskAllocations (
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
);

-- 路径规划表
CREATE TABLE PathPlanings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    planName TEXT NOT NULL,
    coordinationName TEXT NOT NULL,
    uavName TEXT NOT NULL,
    relatedTask TEXT NOT NULL,
    pathPointCount INTEGER NOT NULL,
    status TEXT NOT NULL DEFAULT '待生成',
    createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
    updatedAt TEXT DEFAULT CURRENT_TIMESTAMP
);

-- 路径点表
CREATE TABLE PathPoints (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    pathId INTEGER NOT NULL,
    pointOrder INTEGER NOT NULL,
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    altitude REAL NOT NULL,
    createdAt TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (pathId) REFERENCES PathPlanings(id) ON DELETE CASCADE
);

-- 创建索引
CREATE INDEX idx_taskName ON TaskInfos(taskName);
CREATE INDEX idx_taskAlloc_taskName ON TaskAllocations(taskName);
CREATE INDEX idx_path_uavName ON PathPlanings(uavName);
CREATE INDEX idx_path_relatedTask ON PathPlanings(relatedTask);
CREATE INDEX idx_uav_equipmentId ON UAVInfos(equipmentId);
CREATE INDEX idx_equip_groupId ON EquipmentTypes(groupId);
