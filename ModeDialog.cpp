#include"ModeDialog.h"
#include<QWidget>
ModeDialog::ModeDialog(QWidget *parent):QDialog(parent){
    this->setWindowTitle("选择游戏模式");
    this->setFixedSize(200,100);

    btn_ai_mode = new QPushButton("单人模式",this);
    btn_net_mode = new QPushButton("联机对战",this);
    btn_dual_mode = new QPushButton("双人同机对战",this);

    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->addWidget(btn_ai_mode);
    layout->addWidget(btn_net_mode);
    layout->addWidget(btn_dual_mode);
    this->setLayout(layout);

    connect(btn_ai_mode,&QPushButton::clicked,this,[=](){
        m_mode = AI_MODE;
        accept();//关闭对话框并返回QDialog::Accepted
    });

    connect(btn_net_mode,&QPushButton::clicked,this,[=](){
        m_mode=NET_MODE;
        accept();
    });
    connect(btn_dual_mode,&QPushButton::clicked,this,[=](){
        m_mode = DUAL_MODE;
        accept();
    });


}
