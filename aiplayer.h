#ifndef AIPLAYER_H
#define AIPLAYER_H
#include<QPair>
#include<QVector>
#include<QString>
#include"stdshared.h"
class AIPlayer
{
public:
    AIPlayer(int boardSize);
    QPair<int,int> findBestMove(const QVector<QVector<ROLE>>&board);
private:
    int m_boardSize;
    int matchPattern(const QString &line);
    int evalePoint(int row, int col,const QVector<QVector<ROLE>>&board, ROLE role);
    QString getLineString(int row, int col, int dRow, int dCol,const QVector<QVector<ROLE>>&board, ROLE role);
};

#endif // AIPLAYER_H
