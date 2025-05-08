#include "netmanager.h"
#include <QDataStream>
#include<QHostAddress>

NetManager::NetManager(QObject *parent):QObject(parent){}

void NetManager::startServer(quint16 port)
{
    m_server = new QTcpServer(this);
    connect(m_server,&QTcpServer::newConnection,this,&NetManager::onNewConnection);
    m_server->listen(QHostAddress::Any,port);
}

void NetManager::connectToHost(const QString &host,quint16 port){
    m_socket=new QTcpSocket(this);
    m_socket->connectToHost(host,port);
    connect(m_socket,&QTcpSocket::readyRead,this,&NetManager::onDataReceived);
}

void NetManager::sendMove(int row,int col){
    if(!m_socket) return;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream<<row<<col;
    m_socket->write(data);
}

void NetManager::onNewConnection()
{
    m_socket = m_server->nextPendingConnection();
    connect(m_socket,&QTcpSocket::readyRead,this,&NetManager::onDataReceived);
}

void NetManager::onDataReceived()
{
    QDataStream stream(m_socket);
    int row,col;
    stream>>row>>col;
    emit moveReceived(row,col);//通知外部
}
