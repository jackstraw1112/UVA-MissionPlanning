//
// Created by admin on "2026.04.09 T 19:19:20".
//

#include "FirepowerControl.h"
#include "ui_FirepowerControl.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QProgressBar>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
FirepowerControl::FirepowerControl(QWidget *parent)
    : QWidget(parent), ui(new Ui::FirepowerControl)
{
    ui->setupUi(this);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 关联信号与槽函数
    initConnect();
}

/**
 * @brief 初始化参数
 * @details 预留扩展
 */
void FirepowerControl::initParams()
{
}

/**
 * @brief 初始化对象
 * @details 数据、视图
 */
void FirepowerControl::initObject()
{
    // 生成测试数据
    generateTestData();

    // 显示数据
    displayData();
}

/**
 * @brief 关联信号与槽函数
 */
void FirepowerControl::initConnect()
{
    // 无需按钮连接，仅添加数据
}

/**
 * @brief 析构函数
 */
FirepowerControl::~FirepowerControl()
{
    delete ui;
}

/**
 * @brief 生成测试数据
 * @details 生成防空火力测试数据
 */
void FirepowerControl::generateTestData()
{
    // 清空数据列表
    m_firepowerData.clear();

    // 添加防空火力数据
    FirepowerItem Firepower;
    Firepower.name = QString::fromUtf8("红旗-16 防空系统");
    Firepower.status = QString::fromUtf8("active");
    Firepower.type = QString::fromUtf8("中程防空");
    Firepower.range = QString::fromUtf8("40km");
    Firepower.ammo = QString::fromUtf8("16枚");
    Firepower.response = QString::fromUtf8("8s");
    Firepower.intercept = QString::fromUtf8("95%");
    m_firepowerData.append(Firepower);

    FirepowerItem Firepower1;
    Firepower.name = QString::fromUtf8("PGZ-07 自行高炮");
    Firepower.status = QString::fromUtf8("standby");
    Firepower.type = QString::fromUtf8("末端防御");
    Firepower.range = QString::fromUtf8("3km");
    Firepower.ammo = QString::fromUtf8("400发");
    Firepower.response = QString::fromUtf8("3s");
    Firepower.intercept = QString::fromUtf8("85%");
    m_firepowerData.append(Firepower1);

    FirepowerItem Firepower2;
    Firepower.name = QString::fromUtf8("道尔-M1 防空车");
    Firepower.status = QString::fromUtf8("active");
    Firepower.type = QString::fromUtf8("低空防御");
    Firepower.range = QString::fromUtf8("15km");
    Firepower.ammo = QString::fromUtf8("8枚");
    Firepower.response = QString::fromUtf8("5s");
    Firepower.intercept = QString::fromUtf8("90%");
    m_firepowerData.append(Firepower2);

    FirepowerItem Firepower3;
    Firepower.name = QString::fromUtf8("便携式防空导弹");
    Firepower.status = QString::fromUtf8("standby");
    Firepower.type = QString::fromUtf8("单兵武器");
    Firepower.range = QString::fromUtf8("5km");
    Firepower.ammo = QString::fromUtf8("2枚");
    Firepower.response = QString::fromUtf8("10s");
    Firepower.intercept = QString::fromUtf8("80%");
    m_firepowerData.append(Firepower3);

    FirepowerItem Firepower4;
    Firepower.name = QString::fromUtf8("电子对抗系统");
    Firepower.status = QString::fromUtf8("disabled");
    Firepower.type = QString::fromUtf8("软杀伤");
    Firepower.range = QString::fromUtf8("20km");
    Firepower.ammo = QString::fromUtf8("压制/欺骗");
    Firepower.response = QString::fromUtf8("-");
    Firepower.intercept = QString::fromUtf8("-");
    m_firepowerData.append(Firepower4);

}

/**
 * @brief 显示数据
 * @details 将防空火力数据显示到界面中
 */
void FirepowerControl::displayData()
{
    // 清空布局
    QLayoutItem *item;
    while ((item = ui->verticalLayout_3->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // 添加所有设备项
    for (const auto &firepower : m_firepowerData) {
        addFirepowerItem(firepower);
    }
}

/**
 * @brief 添加设备项
 * @details 根据设备数据创建并添加设备项到界面
 * @param item 设备数据
 */
void FirepowerControl::addFirepowerItem(const FirepowerItem &item)
{
    // 创建设备项框架
    QFrame *firepowerItem = new QFrame;
    QVBoxLayout *itemLayout = new QVBoxLayout(firepowerItem);
    itemLayout->setContentsMargins(0, 0, 0, 0);
    itemLayout->setSpacing(8);

    // 设备头部
    QWidget *headerWidget = new QWidget;
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);

    // 设备名称
    QLabel *nameLabel = new QLabel(item.name);

    // 设备状态
    QLabel *statusLabel = new QLabel;
    
    if (item.status == "active") {
        statusLabel->setText("在线");
    } else if (item.status == "standby") {
        statusLabel->setText("待机");
    } else {
        statusLabel->setText("离线");
    }

    headerLayout->addWidget(nameLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(statusLabel);

    itemLayout->addWidget(headerWidget);

    // 参数网格
    QGridLayout *paramLayout = new QGridLayout;
    paramLayout->setSpacing(6);

    // 类型
    QFrame *typeBox = new QFrame;
    QVBoxLayout *typeBoxLayout = new QVBoxLayout(typeBox);
    typeBoxLayout->setContentsMargins(0, 0, 0, 0);
    typeBoxLayout->setSpacing(2);
    QLabel *typeLabel = new QLabel("类型");
    QLabel *typeValue = new QLabel(item.type);
    typeBoxLayout->addWidget(typeLabel);
    typeBoxLayout->addWidget(typeValue);

    // 射程
    QFrame *rangeBox = new QFrame;
    QVBoxLayout *rangeBoxLayout = new QVBoxLayout(rangeBox);
    rangeBoxLayout->setContentsMargins(0, 0, 0, 0);
    rangeBoxLayout->setSpacing(2);
    QLabel *rangeLabel = new QLabel("射程");
    QLabel *rangeValue = new QLabel(item.range);
    rangeBoxLayout->addWidget(rangeLabel);
    rangeBoxLayout->addWidget(rangeValue);

    // 弹药/模式
    QFrame *ammoBox = new QFrame;
    QVBoxLayout *ammoBoxLayout = new QVBoxLayout(ammoBox);
    ammoBoxLayout->setContentsMargins(0, 0, 0, 0);
    ammoBoxLayout->setSpacing(2);
    QLabel *ammoLabel = new QLabel(item.ammo.contains("枚") ? "弹药" : "模式");
    QLabel *ammoValue = new QLabel(item.ammo);
    ammoBoxLayout->addWidget(ammoLabel);
    ammoBoxLayout->addWidget(ammoValue);

    // 响应
    QFrame *responseBox = new QFrame;
    QVBoxLayout *responseBoxLayout = new QVBoxLayout(responseBox);
    responseBoxLayout->setContentsMargins(0, 0, 0, 0);
    responseBoxLayout->setSpacing(2);
    QLabel *responseLabel = new QLabel("响应");
    QLabel *responseValue = new QLabel(item.response);
    responseBoxLayout->addWidget(responseLabel);
    responseBoxLayout->addWidget(responseValue);

    paramLayout->addWidget(typeBox, 0, 0);
    paramLayout->addWidget(rangeBox, 0, 1);
    paramLayout->addWidget(ammoBox, 1, 0);
    paramLayout->addWidget(responseBox, 1, 1);

    itemLayout->addLayout(paramLayout);

    // 拦截概率
    QFrame *rangeFrame = new QFrame;
    QVBoxLayout *rangeFrameLayout = new QVBoxLayout(rangeFrame);
    rangeFrameLayout->setContentsMargins(0, 0, 0, 0);
    rangeFrameLayout->setSpacing(4);

    QLabel *rangeTitle = new QLabel("拦截概率");

    QFrame *rangeBar = new QFrame;
    QHBoxLayout *rangeBarLayout = new QHBoxLayout(rangeBar);
    rangeBarLayout->setContentsMargins(0, 0, 0, 0);

    QFrame *rangeFill = new QFrame;
    
    // 设置填充宽度
    if (item.intercept != "-") {
        int percent = QString(item.intercept).replace("%", "").toInt();
        rangeFill->setFixedWidth(percent * rangeBar->width() / 100);
    }

    rangeBarLayout->addWidget(rangeFill);

    rangeFrameLayout->addWidget(rangeTitle);
    rangeFrameLayout->addWidget(rangeBar);

    itemLayout->addWidget(rangeFrame);

    // 添加到布局
    ui->verticalLayout_3->addWidget(firepowerItem);
}

/**
 * @brief 根据名称查找索引
 * @param container 数据容器
 * @param name 目标名称
 * @return 找到的索引，未找到返回 -1
 */
int FirepowerControl::findIndexByName(const QVector<FirepowerItem> &container, const QString &name)
{
    for (int i = 0; i < container.size(); ++i)
    {
        if (container.at(i).name == name)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 添加防空火力数据
 * @param data 防空火力对象
 * @details 追加到防空火力缓存并更新显示。
 */
void FirepowerControl::addDataImpl(const FirepowerItem &data)
{
    m_firepowerData.append(data);
    displayData();
}

/**
 * @brief 更新防空火力数据
 * @param data 防空火力对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void FirepowerControl::updateDataImpl(const FirepowerItem &data)
{
    const int row = findIndexByName(m_firepowerData, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_firepowerData[row] = data;
    displayData();
}

/**
 * @brief 删除防空火力数据
 * @param name 目标名称
 * @details 从防空火力缓存和界面中同步删除同名记录。
 */
void FirepowerControl::deleteFirepowerDataByName(const QString &name)
{
    const int row = findIndexByName(m_firepowerData, name);
    if (row < 0)
    {
        return;
    }
    m_firepowerData.remove(row);
    displayData();
}