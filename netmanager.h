#pragma once
#ifndef NETMANAGER_H
#define NETMANAGER_H

#endif // NETMANAGER_H
#include <QObject>
#include<QTcpSocket>
#include<QTcpServer>

class NetManager:public QObject{
    Q_OBJECT
public:
    explicit NetManager(QObject *parent=nullptr);

    //启动服务器(主机)
    void startServer(quint16 port);

    //加入服务器(客户端)
    void connectToHost(const QString &host,quint16 port);

    //发送主标
    void sendMove(int row,int col);
signals:
    //收到坐标信号
    void moveReceived(int row,int col);

private slots:
    void onNewConnection();
    void onDataReceived();

private:
    QTcpServer *m_server=nullptr;
    QTcpSocket *m_socket=nullptr;

};

