#include "aiplayer.h"

AIPlayer::AIPlayer(int boardSize):m_boardSize(boardSize) {
}
QPair<int, int> AIPlayer::findBestMove(const QVector<QVector<ROLE>>& board)
{
    for(int row=0;row<m_boardSize;++row){
        for(int col=0;col<m_boardSize;++col){
            if(board[row][col]!=EMPTY){
                continue;
            }
            QVector<QVector<ROLE>> temp = board;
            temp[row][col]=COMPUTER;
            if(matchPattern(getLineString(row,col,1,0,temp,PLAYER))>=100000||
                matchPattern(getLineString(row,col,0,1,temp,PLAYER))>=100000||
                matchPattern(getLineString(row,col,1,1,temp,PLAYER))>=100000||
                matchPattern(getLineString(row,col,1,-1,temp,PLAYER))>=100000)
            {
                return qMakePair(row,col);
            }

        }
    }
    int maxScore = -1;
    QPair<int,int>best(-1,-1);
    for(int row=0;row<m_boardSize;++row){
        for(int col=0;col<m_boardSize;col++){
            if(board[row][col]==EMPTY){
                int score = evalePoint(row,col,board,COMPUTER);
                score+=evalePoint(row,col,board,PLAYER);
                if(score>maxScore){
                    maxScore = score;
                    best = qMakePair(row,col);
                }
            }
        }
    }
    return best;
}
int AIPlayer::evalePoint(int row, int col,const QVector<QVector<ROLE>>&board, ROLE role)
{
    int totalScore =0;
    totalScore+=matchPattern(getLineString(row,col,1,0,board,role));
    totalScore+=matchPattern(getLineString(row,col,0,1,board,role));
    totalScore+=matchPattern(getLineString(row,col,1,-1,board,role));
    totalScore+=matchPattern(getLineString(row,col,1,1,board,role));
    return totalScore;
}
QString AIPlayer::getLineString(int row, int col, int dRow, int dCol,const QVector<QVector<ROLE>>&board, ROLE role)
{
    QString line ="";
    for(int i =-4;i<=4;i++){
        int r =row+dRow*i;
        int c =col+dCol*i;
        if(r<0||r>=m_boardSize||c<0||c>=m_boardSize){
            line+="O";
        }else if(board[r][c]==role){
            line+="X";
        }else if(board[r][c]==EMPTY){
            line+="_";
        }else{
            line+="O";
        }
    }
    return line;
}

int AIPlayer::matchPattern(const QString &line)
{
    static QVector<QPair<QString,int>> patterns ={
                                                    {"_XXXX_",10000},{"XXXX",100000},
                                                    {"OXXXX_",1000},{"_XXXXO",1000},
                                                    {"_XXX_",100},{"_XXX_",100},
                                                    {"OXXX_",50},  {"_XXXO",50},
                                                    {"_XX_",10},{"_XX_",10},
                                                    };
    int score =0;
    for(const auto &p:patterns){
        if(line.contains(p.first)){
            score+=p.second;
        }
    }
    return score;
}
