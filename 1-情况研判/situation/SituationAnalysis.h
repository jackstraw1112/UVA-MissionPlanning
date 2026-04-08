#ifndef SITUATIONANALYSIS_H
#define SITUATIONANALYSIS_H

#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QScrollArea>

// 频谱分析图自定义控件 - 基于QGraphicsView
class SpectrumChart : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SpectrumChart(QWidget *parent = nullptr);
    void updateSpectrumData();
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    
private:
    struct SpectrumBand {
        QString name;
        QString range;
        QColor color;
        int height;
        int signalCount;
        QRectF rect;
        QGraphicsRectItem* rectItem;
        QGraphicsTextItem* labelItem;
    };
    
    QGraphicsScene* scene;
    QList<SpectrumBand> spectrumData;
    int hoveredIndex;
    QPoint tooltipPos;
    QString tooltipText;
    bool showTooltip;
    QGraphicsTextItem* tooltipItem;
    
    void createSpectrumBars();
    void updateTooltip();
};

class SituationAnalysis : public QMainWindow
{
    Q_OBJECT

public:
    explicit SituationAnalysis(QWidget *parent = nullptr);
    ~SituationAnalysis();

private slots:
    void updateSystemTime();
    void onRadarTabChanged(int index);
    void onToggleRadar(bool checked);
    void onToggleComm(bool checked);
    void onToggleCommJammer(bool checked);
    void onToggleRadarJammer(bool checked);
    void onToggleFirepower(bool checked);

private:
    void setupUI();
    void setupHeader();
    void setupRadarSourceList();
    void setupSpectrumAnalysis();
    void setupThreatAssessment();
    void setupFirepowerControl();
    void setupSituationControl();
    void createRadarSourceData();
    void createThreatData();
    void createFirepowerData();
    
    // 头部组件
    QWidget *headerWidget;
    QLabel *titleLabel;
    QLabel *totalTargetsLabel;
    QLabel *highThreatLabel;
    QLabel *firepowerCountLabel;
    QLabel *currentTimeLabel;
    
    // 左侧组件
    QWidget *leftPanel;
    QGroupBox *radarSourceGroup;
    QTabWidget *radarSourceTabs;
    QTableWidget *radarSourceTable;
    QTableWidget *commSourceTable;
    QTableWidget *commJammerTable;
    QTableWidget *radarJammerTable;
    
    // 右侧组件
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QWidget *spectrumWidget;
    QWidget *threatWidget;
    QWidget *firepowerWidget;
    QWidget *threatRulesWidget;
    QWidget *threatListWidget;
    QGroupBox *spectrumGroup;
    QGroupBox *threatGroup;
    QGroupBox *firepowerGroup;
    
    // 态势控制组件
    QGroupBox *situationControlGroup;
    QCheckBox *showRadarCheck;
    QCheckBox *showCommCheck;
    QCheckBox *showCommJammerCheck;
    QCheckBox *showRadarJammerCheck;
    QCheckBox *showFirepowerCheck;
    
    QTimer *systemTimer;
};

#endif // SITUATIONANALYSIS_H