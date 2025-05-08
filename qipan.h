#ifndef QIPAN_H
#define QIPAN_H

#include <QWidget>
#include <QMouseEvent>
#include <QStack>
#include "stdshared.h"
#include "netmanager.h"
#include "gamemode.h"
#include"aiplayer.h"
class QiPan : public QWidget
{
    Q_OBJECT
public:
    explicit QiPan(QWidget *parent = nullptr);
    void setNetManager(NetManager *net,bool isHost);
    GameMode m_mode;
    void setGameMode(GameMode mode);
protected:
    //会话事件
    void paintEvent(QPaintEvent *event) override;
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void currentRoleChange(ROLE role);
    void undoCountChange(int count);
    void gameOverOccurred();
public slots:
    void reStartGame();
    void undostep();
    void exportStepsToFile();
    void setAiMode(bool enabled);
    void importStepsFromFiles();
    void saveGame();
    void loadGame();

private:
    //判断用户是否获胜
    bool isCheckWin(int row,int col,ROLE role);
    int evalePoint(int row,int col,ROLE role);
    QPair<int,int> findBestMove(ROLE role);
    void makeAIMove();
    QString getLineString(int row,int col,int dRow,int dCil,ROLE role);
    int matchPattern(const QString &line);

private:
    //棋盘的尺寸
    static const int m_boardSize =15;
    //每个方格子的尺寸
    static const int m_cellSize = 40;
    //棋盘距离边缘的距离
    static const int m_margin = 20;
    AIPlayer *ai = nullptr;
    //棋盘上的点
    ROLE m_board[m_boardSize][m_boardSize];

    //悬停点坐标
    int m_hoverRow;
    int m_hoverCol;
    bool m_hoverFlag;//是否需要绘制悬停点
    ROLE currentRole;
    bool m_gameover;

    struct step
    {
        int row;
        int col;
        ROLE role;
    };
    //创建记录落子的栈
    QStack<step> m_step;
    int m_undoCount;

    NetManager *m_net = nullptr;
    bool m_isHost = false;
    bool m_myTurn = true;
    bool m_useAi = true;

};

#endif // QIPAN_H
