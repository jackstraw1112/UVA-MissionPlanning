//
// Created by admin on "2026.04.09 T 00:00:00".
//


#include "FirepowerControl.h"
#include "ui_FirepowerControl.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QProgressBar>

FirepowerControl::FirepowerControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirepowerControl)
{
    ui->setupUi(this);

    // Set global style
    setStyleSheet(
        "QFrame#equipmentItem { background-color: #122033; border-radius: 12px; margin: 8px 20px; padding: 20px; border: 1px solid #1A293D; }"
        "QLabel#equipName { font-size: 22px; font-weight: bold; color: #FFFFFF; }"
        "QLabel#statusLabel { background-color: #10B981; border-radius: 16px; padding: 6px 16px; font-size: 16px; font-weight: bold; color: #FFFFFF; }"
        "QLabel#paramKey { font-size: 18px; color: #CBD5E1; background-color: #0F1724; border-radius: 8px; padding: 12px 16px; }"
        "QLabel#paramValue { font-size: 20px; font-weight: bold; color: #38BDF8; background-color: #0F1724; border-radius: 8px; padding: 12px 16px; }"
        "QProgressBar { border: none; border-radius: 8px; background-color: #0A121F; text-align: center; height: 14px; }"
        "QProgressBar::chunk { background-color: #10B981; border-radius: 8px; }"
        "QLabel#equipIcon { background-color: #FBBF24; border-radius: 14px; }"
    );

    // Add equipment function
    auto addEquipment = [&](const QString &name, const QString &type, const QString &range, const QString &ammo, const QString &response, int progress) {
        QFrame *item = new QFrame;
        item->setObjectName("equipmentItem");
        QVBoxLayout *itemLayout = new QVBoxLayout(item);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(20);

        // Equipment top bar
        QWidget *topBar = new QWidget;
        QHBoxLayout *topLayout = new QHBoxLayout(topBar);
        topLayout->setContentsMargins(0, 0, 0, 0);
        topLayout->setSpacing(12);

        QLabel *equipIcon = new QLabel;
        equipIcon->setObjectName("equipIcon");
        equipIcon->setFixedSize(28, 28);
        equipIcon->setAlignment(Qt::AlignCenter);

        QLabel *equipName = new QLabel(name);
        equipName->setObjectName("equipName");

        QLabel *statusLabel = new QLabel("Standby");
        statusLabel->setObjectName("statusLabel");

        topLayout->addWidget(equipIcon);
        topLayout->addWidget(equipName);
        topLayout->addStretch();
        topLayout->addWidget(statusLabel);

        itemLayout->addWidget(topBar);

        // Parameter grid
        QGridLayout *gridLayout = new QGridLayout;
        gridLayout->setSpacing(12);

        QLabel *typeKey = new QLabel("Type");
        typeKey->setObjectName("paramKey");
        QLabel *typeValue = new QLabel(type);
        typeValue->setObjectName("paramValue");

        QLabel *rangeKey = new QLabel("Range");
        rangeKey->setObjectName("paramKey");
        QLabel *rangeValue = new QLabel(range);
        rangeValue->setObjectName("paramValue");

        QLabel *ammoKey = new QLabel("Ammo");
        ammoKey->setObjectName("paramKey");
        QLabel *ammoValue = new QLabel(ammo);
        ammoValue->setObjectName("paramValue");

        QLabel *respKey = new QLabel("Response");
        respKey->setObjectName("paramKey");
        QLabel *respValue = new QLabel(response);
        respValue->setObjectName("paramValue");

        gridLayout->addWidget(typeKey, 0, 0);
        gridLayout->addWidget(typeValue, 1, 0);
        gridLayout->addWidget(rangeKey, 0, 1);
        gridLayout->addWidget(rangeValue, 1, 1);
        gridLayout->addWidget(ammoKey, 2, 0);
        gridLayout->addWidget(ammoValue, 3, 0);
        gridLayout->addWidget(respKey, 2, 1);
        gridLayout->addWidget(respValue, 3, 1);

        itemLayout->addLayout(gridLayout);

        // Intercept probability progress bar
        QVBoxLayout *progressLayout = new QVBoxLayout;
        progressLayout->setSpacing(8);

        QLabel *pgTitle = new QLabel("Intercept Probability");
        pgTitle->setObjectName("paramKey");
        pgTitle->setStyleSheet("background-color: transparent; padding: 0;");

        QProgressBar *equipProgress = new QProgressBar;
        equipProgress->setRange(0, 100);
        equipProgress->setValue(progress);
        equipProgress->setTextVisible(false);

        progressLayout->addWidget(pgTitle);
        progressLayout->addWidget(equipProgress);
        itemLayout->addLayout(progressLayout);

        ui->verticalLayout_3->addWidget(item);
    };

    // Add equipment
    addEquipment("HQ-16 Air Defense System", "Medium Range", "40km", "16 rounds", "8s", 95);
    addEquipment("Tor-M1 Air Defense Vehicle", "Low Altitude", "15km", "8 rounds", "5s", 90);
    addEquipment("PGZ-07 Self-propelled AAA", "Terminal Defense", "3km", "400 rounds", "3s", 88);
    addEquipment("Man-portable Air Defense Missile", "Terminal Defense", "5km", "8 rounds", "5s", 85);
    addEquipment("HQ-9 Long Range Air Defense", "Long Range", "200km", "8 rounds", "10s", 98);
}

FirepowerControl::~FirepowerControl()
{
    delete ui;
}