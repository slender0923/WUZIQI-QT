#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qipan.h"
#include <QLabel>
#include"gamemode.h"
#include <QPushButton>
#include <QVBoxLayout>
class QiPan;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setMode(GameMode mode);
    GameMode m_mode;

private slots:

private:
    Ui::MainWindow *ui;
    QiPan  *m_qipan;
    QLabel *m_statusLabel;
    QLabel *m_rolelabel;
    NetManager *m_net=nullptr;//网络通信管理器
    QPushButton *btn_host=nullptr;
    QPushButton *btn_join=nullptr;
    QPushButton *btn_back=nullptr;
    QVBoxLayout *rightLayout=nullptr;

};
#endif // MAINWINDOW_H
