/********************************************************************************
** Form generated from reading UI file 'SituationAnalysis.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SITUATIONANALYSIS_H
#define SITUATIONANALYSIS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SituationAnalysis
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QWidget *headerWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *titleLabel;
    QSpacerItem *horizontalSpacer;
    QLabel *totalTargetsLabel;
    QLabel *highThreatLabel;
    QLabel *firepowerCountLabel;
    QLabel *currentTimeLabel;
    QVBoxLayout *leftLayout;
    QGroupBox *radarSourceGroup;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *radarSourceTabs;
    QWidget *radarTab;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *radarSourceTable;
    QWidget *commTab;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *commSourceTable;
    QWidget *commJammerTab;
    QVBoxLayout *verticalLayout_5;
    QTableWidget *commJammerTable;
    QWidget *radarJammerTab;
    QVBoxLayout *verticalLayout_6;
    QTableWidget *radarJammerTable;
    QGroupBox *situationControlGroup;
    QVBoxLayout *verticalLayout_7;
    QLabel *label;
    QCheckBox *showRadarCheck;
    QCheckBox *showCommCheck;
    QCheckBox *showCommJammerCheck;
    QCheckBox *showRadarJammerCheck;
    QLabel *label_2;
    QCheckBox *showFirepowerCheck;
    QVBoxLayout *rightLayout;
    QGroupBox *spectrumGroup;
    QVBoxLayout *verticalLayout_8;
    QWidget *spectrumWidget;
    QGroupBox *threatGroup;
    QVBoxLayout *verticalLayout_9;
    QWidget *threatWidget;
    QGroupBox *firepowerGroup;
    QVBoxLayout *verticalLayout_10;
    QWidget *firepowerWidget;

    void setupUi(QMainWindow *SituationAnalysis)
    {
        if (SituationAnalysis->objectName().isEmpty())
            SituationAnalysis->setObjectName(QString::fromUtf8("SituationAnalysis"));
        SituationAnalysis->resize(1000, 800);
        SituationAnalysis->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0a0f1e, stop:1 #03060c);\n"
"    color: #eef2ff;\n"
"    font-family: 'Inter', 'Microsoft YaHei', sans-serif;\n"
"}\n"
"\n"
"/* \345\215\241\347\211\207\346\240\267\345\274\217 */\n"
"QGroupBox {\n"
"    background: rgba(8, 16, 30, 190);\n"
"    border: 1px solid rgba(72, 187, 255, 77);\n"
"    border-radius: 16px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    padding: 12px 16px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(20, 40, 65, 230), stop:1 rgba(10, 22, 45, 180));\n"
"    color: #eef2ff;\n"
"    font-weight: 600;\n"
"    font-size: 1rem;\n"
"    border-bottom: 1px solid rgba(72, 187, 255, 77);\n"
"    border-top-left-radius: 16px;\n"
"    border-top-right-radius: 16px;\n"
"}\n"
"\n"
"/* \350\241\250\346\240\274\346\240\267\345\274\217 */\n"
"QTableWidget {\n"
"    background"
                        ": rgba(12, 22, 40, 77);\n"
"    border: none;\n"
"    gridline-color: rgba(72, 120, 160, 51);\n"
"    selection-background-color: rgba(30, 70, 110, 102);\n"
"    font-size: 13px;\n"
"    border-bottom-left-radius: 16px;\n"
"    border-bottom-right-radius: 16px;\n"
"}\n"
"\n"
"QTableWidget::item {\n"
"    padding: 10px 12px;\n"
"    border-bottom: 1px solid rgba(72, 120, 160, 51);\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
"    background-color: rgba(30, 70, 110, 153);\n"
"}\n"
"\n"
"QTableWidget::item:hover {\n"
"    background-color: rgba(30, 70, 110, 102);\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background: rgba(12, 26, 45, 230);\n"
"    color: #c2e0ff;\n"
"    font-weight: 600;\n"
"    padding: 10px 12px;\n"
"    border-bottom: 1px solid rgba(72, 187, 255, 102);\n"
"    font-size: 12px;\n"
"}\n"
"\n"
"/* \346\240\207\347\255\276\351\241\265\346\240\267\345\274\217 */\n"
"QTabWidget::pane {\n"
"    border: 1px solid rgba(72, 187, 255, 77);\n"
"    border-radius: 16px;\n"
"    background: rgba(8, 1"
                        "6, 30, 190);\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background: rgba(30, 50, 70, 128);\n"
"    color: #bbd9ff;\n"
"    padding: 8px 10px;\n"
"    margin: 6px 3px;\n"
"    border-radius: 8px;\n"
"    font-size: 12px;\n"
"    font-weight: 500;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2c577c, stop:1 #1a3a55);\n"
"    color: #fff;\n"
"    box-shadow: 0 2px 8px rgba(91, 192, 255, 77);\n"
"}\n"
"\n"
"QTabBar::tab:hover {\n"
"    background: rgba(50, 80, 110, 153);\n"
"}\n"
"\n"
"QTabBar::QToolButton {\n"
"    background: rgba(30, 50, 70, 130);\n"
"    color: #bbd9ff;\n"
"}\n"
"\n"
"/* \346\214\211\351\222\256\346\240\267\345\274\217 */\n"
"QPushButton {\n"
"    background: rgba(30, 50, 70, 128);\n"
"    color: #bbd9ff;\n"
"    border: none;\n"
"    border-radius: 8px;\n"
"    padding: 8px 16px;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: rgba(50, 80, 110, 153);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background:"
                        " rgba(20, 40, 60, 204);\n"
"}\n"
"\n"
"/* \345\244\215\351\200\211\346\241\206\346\240\267\345\274\217 - \345\274\200\345\205\263\346\240\267\345\274\217 */\n"
"QCheckBox {\n"
"    color: #eef2ff;\n"
"    font-size: 13px;\n"
"    spacing: 6px;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: 40px;\n"
"    height: 20px;\n"
"    border-radius: 10px;\n"
"    background: rgba(30, 50, 70, 204);\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked {\n"
"    background: rgba(30, 50, 70, 204);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
"    background: rgba(91, 192, 255, 128);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:hover {\n"
"    background: rgba(91, 192, 255, 153);\n"
"}\n"
"\n"
"QCheckBox::indicator::after {\n"
"    content: \"\";\n"
"    position: absolute;\n"
"    width: 14px;\n"
"    height: 14px;\n"
"    left: 3px;\n"
"    bottom: 3px;\n"
"    background: #8E9CB9;\n"
"    border-radius: 50%;\n"
"    transition: 0.3s;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked::after {\n"
"    left: 23px;\n"
"    backgr"
                        "ound: #5BC0FF;\n"
"}\n"
"\n"
"/* \350\277\233\345\272\246\346\235\241\346\240\267\345\274\217 */\n"
"QProgressBar {\n"
"    border: none;\n"
"    background: rgba(0, 0, 0, 77);\n"
"    border-radius: 3px;\n"
"    text-align: center;\n"
"    color: #5BC0FF;\n"
"    font-weight: 500;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #5BC0FF, stop:1 #2E8B57);\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"/* \346\240\207\347\255\276\346\240\267\345\274\217 */\n"
"QLabel {\n"
"    color: #eef2ff;\n"
"    font-size: 12px;\n"
"}\n"
"\n"
"QLabel[objectName^=\"stat\"] {\n"
"    font-size: 22px;\n"
"    font-weight: 700;\n"
"    color: #5BC0FF;\n"
"}\n"
"\n"
"QLabel[objectName^=\"title\"] {\n"
"    font-size: 24px;\n"
"    font-weight: 600;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFFFFF, stop:1 #80CFFF);\n"
"    color: transparent;\n"
"    background-clip: text;\n"
"}\n"
"\n"
"/* \346\273\232\345\212\250\346\235\241\346\240\267\345\274\217 */"
                        "\n"
"QScrollBar:vertical {\n"
"    width: 6px;\n"
"    background: rgba(0, 0, 0, 51);\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background: rgba(91, 192, 255, 77);\n"
"    border-radius: 3px;\n"
"    min-height: 30px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: rgba(91, 192, 255, 128);\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"    height: 0px;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    height: 6px;\n"
"    background: rgba(0, 0, 0, 51);\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"    background: rgba(91, 192, 255, 77);\n"
"    border-radius: 3px;\n"
"    min-width: 30px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background: rgba(91, 192, 255, 128);\n"
"}\n"
"\n"
"/* \345\210\206\347\273\204\346\240\207\351\242\230\346\240\267\345\274\217 */\n"
"QWidget[objectName=\"headerWidget\"] {\n"
"    background: rgba(12, 20, 35, 204);\n"
"    border: 1px soli"
                        "d rgba(72, 187, 255, 77);\n"
"    border-radius: 16px;\n"
"}"));
        centralwidget = new QWidget(SituationAnalysis);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(16);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(16, 16, 16, 16);
        headerWidget = new QWidget(centralwidget);
        headerWidget->setObjectName(QString::fromUtf8("headerWidget"));
        horizontalLayout = new QHBoxLayout(headerWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        titleLabel = new QLabel(headerWidget);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));

        horizontalLayout->addWidget(titleLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        totalTargetsLabel = new QLabel(headerWidget);
        totalTargetsLabel->setObjectName(QString::fromUtf8("totalTargetsLabel"));
        totalTargetsLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(totalTargetsLabel);

        highThreatLabel = new QLabel(headerWidget);
        highThreatLabel->setObjectName(QString::fromUtf8("highThreatLabel"));
        highThreatLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(highThreatLabel);

        firepowerCountLabel = new QLabel(headerWidget);
        firepowerCountLabel->setObjectName(QString::fromUtf8("firepowerCountLabel"));
        firepowerCountLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(firepowerCountLabel);

        currentTimeLabel = new QLabel(headerWidget);
        currentTimeLabel->setObjectName(QString::fromUtf8("currentTimeLabel"));
        currentTimeLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(currentTimeLabel);


        gridLayout->addWidget(headerWidget, 0, 0, 1, 2);

        leftLayout = new QVBoxLayout();
        leftLayout->setSpacing(16);
        leftLayout->setObjectName(QString::fromUtf8("leftLayout"));
        radarSourceGroup = new QGroupBox(centralwidget);
        radarSourceGroup->setObjectName(QString::fromUtf8("radarSourceGroup"));
        verticalLayout_2 = new QVBoxLayout(radarSourceGroup);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        radarSourceTabs = new QTabWidget(radarSourceGroup);
        radarSourceTabs->setObjectName(QString::fromUtf8("radarSourceTabs"));
        radarSourceTabs->setTabPosition(QTabWidget::North);
        radarSourceTabs->setTabShape(QTabWidget::Rounded);
        radarTab = new QWidget();
        radarTab->setObjectName(QString::fromUtf8("radarTab"));
        verticalLayout_3 = new QVBoxLayout(radarTab);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        radarSourceTable = new QTableWidget(radarTab);
        radarSourceTable->setObjectName(QString::fromUtf8("radarSourceTable"));
        radarSourceTable->setRowCount(6);
        radarSourceTable->setColumnCount(5);

        verticalLayout_3->addWidget(radarSourceTable);

        radarSourceTabs->addTab(radarTab, QString());
        commTab = new QWidget();
        commTab->setObjectName(QString::fromUtf8("commTab"));
        verticalLayout_4 = new QVBoxLayout(commTab);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        commSourceTable = new QTableWidget(commTab);
        commSourceTable->setObjectName(QString::fromUtf8("commSourceTable"));
        commSourceTable->setRowCount(3);
        commSourceTable->setColumnCount(5);

        verticalLayout_4->addWidget(commSourceTable);

        radarSourceTabs->addTab(commTab, QString());
        commJammerTab = new QWidget();
        commJammerTab->setObjectName(QString::fromUtf8("commJammerTab"));
        verticalLayout_5 = new QVBoxLayout(commJammerTab);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        commJammerTable = new QTableWidget(commJammerTab);
        commJammerTable->setObjectName(QString::fromUtf8("commJammerTable"));
        commJammerTable->setRowCount(2);
        commJammerTable->setColumnCount(5);

        verticalLayout_5->addWidget(commJammerTable);

        radarSourceTabs->addTab(commJammerTab, QString());
        radarJammerTab = new QWidget();
        radarJammerTab->setObjectName(QString::fromUtf8("radarJammerTab"));
        verticalLayout_6 = new QVBoxLayout(radarJammerTab);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        radarJammerTable = new QTableWidget(radarJammerTab);
        radarJammerTable->setObjectName(QString::fromUtf8("radarJammerTable"));
        radarJammerTable->setRowCount(1);
        radarJammerTable->setColumnCount(5);

        verticalLayout_6->addWidget(radarJammerTable);

        radarSourceTabs->addTab(radarJammerTab, QString());

        verticalLayout_2->addWidget(radarSourceTabs);


        leftLayout->addWidget(radarSourceGroup);

        situationControlGroup = new QGroupBox(centralwidget);
        situationControlGroup->setObjectName(QString::fromUtf8("situationControlGroup"));
        verticalLayout_7 = new QVBoxLayout(situationControlGroup);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        label = new QLabel(situationControlGroup);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_7->addWidget(label);

        showRadarCheck = new QCheckBox(situationControlGroup);
        showRadarCheck->setObjectName(QString::fromUtf8("showRadarCheck"));
        showRadarCheck->setChecked(true);

        verticalLayout_7->addWidget(showRadarCheck);

        showCommCheck = new QCheckBox(situationControlGroup);
        showCommCheck->setObjectName(QString::fromUtf8("showCommCheck"));
        showCommCheck->setChecked(true);

        verticalLayout_7->addWidget(showCommCheck);

        showCommJammerCheck = new QCheckBox(situationControlGroup);
        showCommJammerCheck->setObjectName(QString::fromUtf8("showCommJammerCheck"));
        showCommJammerCheck->setChecked(true);

        verticalLayout_7->addWidget(showCommJammerCheck);

        showRadarJammerCheck = new QCheckBox(situationControlGroup);
        showRadarJammerCheck->setObjectName(QString::fromUtf8("showRadarJammerCheck"));
        showRadarJammerCheck->setChecked(true);

        verticalLayout_7->addWidget(showRadarJammerCheck);

        label_2 = new QLabel(situationControlGroup);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_7->addWidget(label_2);

        showFirepowerCheck = new QCheckBox(situationControlGroup);
        showFirepowerCheck->setObjectName(QString::fromUtf8("showFirepowerCheck"));
        showFirepowerCheck->setChecked(true);

        verticalLayout_7->addWidget(showFirepowerCheck);


        leftLayout->addWidget(situationControlGroup);


        gridLayout->addLayout(leftLayout, 1, 0, 1, 1);

        rightLayout = new QVBoxLayout();
        rightLayout->setSpacing(16);
        rightLayout->setObjectName(QString::fromUtf8("rightLayout"));
        spectrumGroup = new QGroupBox(centralwidget);
        spectrumGroup->setObjectName(QString::fromUtf8("spectrumGroup"));
        verticalLayout_8 = new QVBoxLayout(spectrumGroup);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        spectrumWidget = new QWidget(spectrumGroup);
        spectrumWidget->setObjectName(QString::fromUtf8("spectrumWidget"));
        spectrumWidget->setMinimumSize(QSize(0, 200));

        verticalLayout_8->addWidget(spectrumWidget);


        rightLayout->addWidget(spectrumGroup);

        threatGroup = new QGroupBox(centralwidget);
        threatGroup->setObjectName(QString::fromUtf8("threatGroup"));
        verticalLayout_9 = new QVBoxLayout(threatGroup);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        threatWidget = new QWidget(threatGroup);
        threatWidget->setObjectName(QString::fromUtf8("threatWidget"));
        threatWidget->setMinimumSize(QSize(0, 150));

        verticalLayout_9->addWidget(threatWidget);


        rightLayout->addWidget(threatGroup);

        firepowerGroup = new QGroupBox(centralwidget);
        firepowerGroup->setObjectName(QString::fromUtf8("firepowerGroup"));
        verticalLayout_10 = new QVBoxLayout(firepowerGroup);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        firepowerWidget = new QWidget(firepowerGroup);
        firepowerWidget->setObjectName(QString::fromUtf8("firepowerWidget"));
        firepowerWidget->setMinimumSize(QSize(0, 200));

        verticalLayout_10->addWidget(firepowerWidget);


        rightLayout->addWidget(firepowerGroup);


        gridLayout->addLayout(rightLayout, 1, 1, 1, 1);

        SituationAnalysis->setCentralWidget(centralwidget);

        retranslateUi(SituationAnalysis);

        QMetaObject::connectSlotsByName(SituationAnalysis);
    } // setupUi

    void retranslateUi(QMainWindow *SituationAnalysis)
    {
        SituationAnalysis->setWindowTitle(QApplication::translate("SituationAnalysis", "\345\217\215\350\276\220\345\260\204\346\227\240\344\272\272\346\234\272\344\273\277\347\234\237\347\263\273\347\273\237 - \346\200\201\345\212\277\345\210\206\346\236\220", nullptr));
        titleLabel->setText(QApplication::translate("SituationAnalysis", "\345\217\215\350\276\220\345\260\204\346\227\240\344\272\272\346\234\272\344\273\277\347\234\237\347\263\273\347\273\237", nullptr));
        totalTargetsLabel->setText(QApplication::translate("SituationAnalysis", "12", nullptr));
        totalTargetsLabel->setObjectName(QApplication::translate("SituationAnalysis", "statLabel", nullptr));
        highThreatLabel->setText(QApplication::translate("SituationAnalysis", "4", nullptr));
        highThreatLabel->setObjectName(QApplication::translate("SituationAnalysis", "statLabel", nullptr));
        firepowerCountLabel->setText(QApplication::translate("SituationAnalysis", "5", nullptr));
        firepowerCountLabel->setObjectName(QApplication::translate("SituationAnalysis", "statLabel", nullptr));
        currentTimeLabel->setText(QApplication::translate("SituationAnalysis", "--:--:--", nullptr));
        currentTimeLabel->setObjectName(QApplication::translate("SituationAnalysis", "statLabel", nullptr));
        radarSourceGroup->setTitle(QApplication::translate("SituationAnalysis", "\350\276\220\345\260\204\346\272\220\345\210\227\350\241\250", nullptr));
        radarSourceTabs->setTabText(radarSourceTabs->indexOf(radarTab), QApplication::translate("SituationAnalysis", "\351\233\267\350\276\276", nullptr));
        radarSourceTabs->setTabText(radarSourceTabs->indexOf(commTab), QApplication::translate("SituationAnalysis", "\347\224\265\345\217\260", nullptr));
        radarSourceTabs->setTabText(radarSourceTabs->indexOf(commJammerTab), QApplication::translate("SituationAnalysis", "\351\200\232\344\277\241\345\257\271\346\212\227", nullptr));
        radarSourceTabs->setTabText(radarSourceTabs->indexOf(radarJammerTab), QApplication::translate("SituationAnalysis", "\351\233\267\350\276\276\345\257\271\346\212\227", nullptr));
        situationControlGroup->setTitle(QApplication::translate("SituationAnalysis", "\346\200\201\345\212\277\346\216\247\345\210\266", nullptr));
        label->setText(QApplication::translate("SituationAnalysis", "\350\276\220\345\260\204\346\272\220\345\250\201\345\212\233\350\214\203\345\233\264", nullptr));
        showRadarCheck->setText(QApplication::translate("SituationAnalysis", "\351\233\267\350\276\276", nullptr));
        showCommCheck->setText(QApplication::translate("SituationAnalysis", "\347\224\265\345\217\260", nullptr));
        showCommJammerCheck->setText(QApplication::translate("SituationAnalysis", "\351\200\232\344\277\241\345\257\271\346\212\227", nullptr));
        showRadarJammerCheck->setText(QApplication::translate("SituationAnalysis", "\351\233\267\350\276\276\345\257\271\346\212\227", nullptr));
        label_2->setText(QApplication::translate("SituationAnalysis", "\351\230\262\346\216\247\347\201\253\345\212\233", nullptr));
        showFirepowerCheck->setText(QApplication::translate("SituationAnalysis", "\351\230\262\346\216\247\347\201\253\345\212\233\346\230\276\347\244\272", nullptr));
        spectrumGroup->setTitle(QApplication::translate("SituationAnalysis", "\351\242\221\350\260\261\345\210\206\346\236\220", nullptr));
        threatGroup->setTitle(QApplication::translate("SituationAnalysis", "\345\250\201\350\203\201\350\257\204\344\274\260", nullptr));
        firepowerGroup->setTitle(QApplication::translate("SituationAnalysis", "\351\230\262\346\216\247\347\201\253\345\212\233", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SituationAnalysis: public Ui_SituationAnalysis {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SITUATIONANALYSIS_H
