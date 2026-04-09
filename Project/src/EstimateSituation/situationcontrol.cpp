//
// Created by 15955 on 2026/4/8.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SituationControl.h" resolved

#include "SituationControl.h"
#include "ui_SituationControl.h"

SituationControl::SituationControl(QWidget *parent) : QWidget(parent)
            , ui(new Ui::SituationControl)
{
    ui->setupUi(this);
    
    // 替换普通 QCheckBox 为带动画的 AnimatedSwitch
    // 雷达开关
    AnimatedSwitch *radarSwitch = new AnimatedSwitch();
    radarSwitch->setFixedSize(ui->radarSwitch->size());
    radarSwitch->setChecked(ui->radarSwitch->isChecked());
    ui->radarLayout->replaceWidget(ui->radarSwitch, radarSwitch);
    ui->radarSwitch->deleteLater();
    
    // 电台开关
    AnimatedSwitch *radioSwitch = new AnimatedSwitch();
    radioSwitch->setFixedSize(ui->radioSwitch->size());
    radioSwitch->setChecked(ui->radioSwitch->isChecked());
    ui->radioLayout->replaceWidget(ui->radioSwitch, radioSwitch);
    ui->radioSwitch->deleteLater();
    
    // 通信对抗开关
    AnimatedSwitch *commJamSwitch = new AnimatedSwitch();
    commJamSwitch->setFixedSize(ui->commJamSwitch->size());
    commJamSwitch->setChecked(ui->commJamSwitch->isChecked());
    ui->commJamLayout->replaceWidget(ui->commJamSwitch, commJamSwitch);
    ui->commJamSwitch->deleteLater();
    
    // 雷达对抗开关
    AnimatedSwitch *radarJamSwitch = new AnimatedSwitch();
    radarJamSwitch->setFixedSize(ui->radarJamSwitch->size());
    radarJamSwitch->setChecked(ui->radarJamSwitch->isChecked());
    ui->radarJamLayout->replaceWidget(ui->radarJamSwitch, radarJamSwitch);
    ui->radarJamSwitch->deleteLater();
    
    // 防控火力显示开关
    AnimatedSwitch *defenseFireDisplaySwitch = new AnimatedSwitch();
    defenseFireDisplaySwitch->setFixedSize(ui->defenseFireDisplaySwitch->size());
    defenseFireDisplaySwitch->setChecked(ui->defenseFireDisplaySwitch->isChecked());
    ui->defenseFireDisplayLayout->replaceWidget(ui->defenseFireDisplaySwitch, defenseFireDisplaySwitch);
    ui->defenseFireDisplaySwitch->deleteLater();
}

SituationControl::~SituationControl()
{
    delete ui;
}
