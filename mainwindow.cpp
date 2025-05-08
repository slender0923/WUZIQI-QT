#include "mainwindow.h"
#include "ui_mainwindow.h"
//垂直布局
#include <QVBoxLayout>
#include   <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include "ModeDialog.h"
#include <QinputDialog>
#include<QNetworkInterface>
#include<QMenu>
#include<QMenuBar>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置窗口名称
    setWindowTitle("五子棋");
    //设置窗口固定大小
    setFixedSize(800,640);
    //创建棋盘对象
    //m_qipan= new QiPan(this);

    // QMenuBar *bar = menuBar();
    // QMenu *menu = bar->addMenu("游戏");
    // QAction *backAction = new QAction ("返回主界面",this);
    // menu->addAction(backAction);
    // connect(backAction,&QAction::triggered,this,[=](){
    //     this->hide();
    //     ModeDialog dlg;
    //     if(dlg.exec() == QDialog::Accepted){
    //         setMode(dlg.selectMode());
    //         this->show();
    //         m_qipan->reStartGame();
    //     }
    // });

    //设置布局
    QWidget * centerWidget  = new QWidget(this);
    setCentralWidget(centerWidget);
    //主布局：水平布局，左边是棋盘，右边是控件
    QHBoxLayout *mainLayout = new QHBoxLayout(centerWidget);

    //左侧：棋盘
    m_qipan = new QiPan(centerWidget);
    mainLayout->addWidget(m_qipan, 1);  // stretch 为 1，自动拉伸

    rightLayout = new QVBoxLayout();
    mainLayout->addLayout(rightLayout);

    // 添加标签（当前方）
    m_rolelabel = new QLabel("当前方:玩家", this);
    m_rolelabel->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(m_rolelabel);

    // 添加按钮
    QPushButton *btn_restart = new QPushButton("重新开始", this);
    QPushButton *btn_undo = new QPushButton("悔棋",this);
    QPushButton *btn_Export =new QPushButton("导出棋谱",this);
    btn_host = new QPushButton("创建房间");
    btn_join = new QPushButton("加入房间");
    btn_back = new QPushButton("返回菜单");

    QPushButton *btn_Import = new QPushButton("导入棋谱",this);
    rightLayout->addWidget(btn_Import);
    connect(btn_Import,&QPushButton::clicked,m_qipan,&QiPan::importStepsFromFiles);
    QPushButton *btn_save = new QPushButton("保存对局",this);
    rightLayout->addWidget(btn_save);
    connect(btn_save,&QPushButton::clicked,m_qipan,&QiPan::saveGame);
    QPushButton *btn_load = new QPushButton("加载对局",this);
    rightLayout->addWidget(btn_load);
    connect(btn_load,&QPushButton::clicked,m_qipan,&QiPan::loadGame);
    rightLayout->addWidget(btn_restart);
    rightLayout->addWidget(btn_undo);
    rightLayout->addWidget(btn_Export);
    rightLayout->addWidget(btn_host);
    rightLayout->addWidget(btn_join);
    rightLayout->addWidget(btn_back);
    btn_back->hide();

    m_statusLabel = new QLabel("游戏进行中",this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(m_statusLabel);

    // 添加弹性空间，把控件推上去
    rightLayout->addStretch();
    connect(btn_host,&QPushButton::clicked,this,[=](){
        m_net = new NetManager(this);
        m_net->startServer(6666);
        QMessageBox::information(this,"联机","房间已创建等待联机");
        m_qipan->setNetManager(m_net,true);
    });
    connect(btn_join,&QPushButton::clicked,this,[=](){
        QString ip = QInputDialog::getText(this,"输入ip","请输入房主ip");
        if(!ip.isEmpty()){
            m_net= new NetManager(this);
            m_net->connectToHost(ip,6666);
            m_qipan->setNetManager(m_net,false);
        }
    });


    connect(btn_back,&QPushButton::clicked,this,[=](){
        this->hide();
        ModeDialog dlg;
        if(dlg.exec() == QDialog::Accepted){
            setMode(dlg.selectMode());
            this->show();
        }
    });

    connect(btn_Export,&QPushButton::clicked,m_qipan,&QiPan::exportStepsToFile);
    connect(btn_undo,&QPushButton::clicked,m_qipan,&QiPan::undostep);
    connect(btn_restart,&QPushButton::clicked,m_qipan,&QiPan::reStartGame);
    connect(m_qipan,&QiPan::currentRoleChange,this,[=](ROLE role){
        if(m_mode==AI_MODE){
            if(role==PLAYER){
                m_rolelabel->setText("当前方:玩家");
            }else{
                m_rolelabel->setText("当前方:电脑");
            }
        }else if(m_mode==DUAL_MODE){
            if(role==PLAYER){
                m_rolelabel->setText("当前方:玩家1");
            }else{
                m_rolelabel->setText("当前方:玩家2");
            }
        }else if(m_mode==NET_MODE){
            if(role==PLAYER){
                m_rolelabel->setText("当前方:玩家1");
            }else{
                m_rolelabel->setText("当前方:玩家2");
            }
        };
    });
    connect(m_qipan,&QiPan::undoCountChange,this,[=](int count){
        btn_undo->setText(QString("悔棋次数剩余%1次").arg(count));
    });

    connect(m_qipan,&QiPan::gameOverOccurred,this,[=](){
        m_statusLabel->setText("游戏结束");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMode(GameMode mode)
{
    m_mode=mode;
    m_qipan->reStartGame();//每次切换模式都清盘

    if(mode==AI_MODE){
        m_qipan->setGameMode(AI_MODE);
        m_qipan->setAiMode(true);
        if(btn_host) btn_host->hide();
        if(btn_join) btn_join->hide();
        btn_back->show();
    }else if(mode==DUAL_MODE){
        m_qipan->setGameMode(DUAL_MODE);
        if(btn_host) btn_host->hide();
        if(btn_join) btn_join->hide();
        btn_back->show();
    }else if(mode==NET_MODE){
        m_qipan->setGameMode(NET_MODE);
        m_qipan->setAiMode(false);
        btn_host->show();
        btn_join->show();
        QString localIP;
        const QList<QHostAddress> &addresses = QNetworkInterface::allAddresses();
        for(const QHostAddress &addr:addresses){
            if(addr.protocol()==QAbstractSocket::IPv4Protocol&&!addr.isLoopback()){
                localIP =addr.toString();
                break;
            }
        }
        QMessageBox::information(this,"主机ip","请对方联机IP"+localIP);
        btn_back->show();
    }
    if(m_statusLabel){
        if(mode==AI_MODE){
            m_statusLabel->setText("模式：AI");
        }else if(mode == DUAL_MODE){
            m_statusLabel->setText("模式：双人对战");
        }else if(mode == NET_MODE){
            m_statusLabel->setText("模式：联机对战");
        };
    }

}



