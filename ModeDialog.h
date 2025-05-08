#ifndef MODEDIALOG_H
#define MODEDIALOG_H
#include <QDialog>
#include<QPushButton>
#include<QVBoxLayout>
#include"gamemode.h"
#endif // MODEDIALOG_H


class ModeDialog:public QDialog{
    Q_OBJECT
public:
    ModeDialog(QWidget *parent = nullptr);
    GameMode selectMode() const {return m_mode;}

private:
    GameMode m_mode;
    QPushButton *btn_ai_mode;
    QPushButton *btn_net_mode;
    QPushButton *btn_dual_mode;
};
