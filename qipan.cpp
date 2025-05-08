#include "qipan.h"
#include <QPainter>
#include <QDebug>
#include<QMessageBox>
#include <QTimer>
#include <QFile>
#include<QTextStream>
#include<QFileDialog>
#include <QRegularExpression>
#include <QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QChar>
QiPan::QiPan(QWidget *parent)
    : QWidget{parent},m_hoverCol(-1),m_hoverRow(-1),
    m_hoverFlag(false),currentRole(PLAYER),m_gameover(false),
    m_undoCount(3)
{
    ai = new AIPlayer(m_boardSize);
   //设置鼠标追踪
    setMouseTracking(true);
   //初始化清空棋盘
   for (int idx=0;idx<m_boardSize;idx++){
        for (int idj=0;idj<m_boardSize;idj++){
           m_board[idx][idj]=EMPTY;
        }
   }
}


/*绘画事件*/
void QiPan::paintEvent(QPaintEvent *event){
    /*创建画家类*/
    QPainter painter(this);

    /*去毛刺 - 抗锯齿*/
    painter.setRenderHint(QPainter::Antialiasing);

    //设置棋盘颜色
    QColor background  = QColor(118,104,122);
    painter.fillRect(rect(),background);

    //绘制棋盘m_cellSize
    for(int idx =0;idx<m_boardSize;idx++){
        //绘制竖线,f1查看帮助
        painter.drawLine(m_margin+idx*m_cellSize,m_margin,
                         m_margin+idx*m_cellSize,m_margin+(m_boardSize-1)*
                         m_cellSize
                         );
        //绘制横线
        painter.drawLine(m_margin,m_margin+idx*m_cellSize,
                         m_margin+(m_boardSize-1)*m_cellSize,m_margin+idx*m_cellSize);
    }

    //绘制悬停点
    if(m_hoverFlag==true&&(m_hoverCol>=0&&m_hoverCol<m_boardSize)
        &&(m_hoverRow>=0&&m_hoverRow<m_boardSize)
        &&m_board[m_hoverRow][m_hoverCol]==EMPTY){
        QColor previewColor = (currentRole == PLAYER)?Qt::black:Qt::white ;
        painter.setBrush(previewColor);
        painter.setOpacity(0.3);
        painter.drawEllipse(QPoint(
            m_margin+m_hoverRow*m_cellSize,
            m_margin+m_hoverCol*m_cellSize),
            m_cellSize/3,m_cellSize/3);
        painter.setOpacity(1.0);
    }

    for(int idx=0;idx<m_boardSize;idx++){
        for(int idj=0;idj<m_boardSize;idj++){
            if(m_board[idx][idj]==PLAYER){
                painter.setBrush(Qt::black);
                painter.drawEllipse(QPoint(m_margin+idx*m_cellSize,m_margin+idj*m_cellSize)
                                    ,m_cellSize/3,m_cellSize/3);
            }else if( m_board[idx][idj]==COMPUTER){
                painter.setBrush(Qt::white);
                painter.drawEllipse(QPoint(m_margin+idx*m_cellSize,m_margin+idj*m_cellSize)
                                    ,m_cellSize/3,m_cellSize/3);
            }
        }
    }
    painter.setPen(Qt::black);
    QFont font =painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    for(int i =0;i<m_boardSize;i++){
        QString num =QString::number(i+1);
        painter.drawText(m_margin-18,m_margin+i*m_cellSize+5,num);
    }
    for(int j = 0;j<m_boardSize;j++){
        QChar ch('A'+j);
        painter.drawText(m_margin+j*m_cellSize-5,m_margin-8,ch);
    }
}

void QiPan::mouseMoveEvent(QMouseEvent *event){
    //绝对坐标转化为棋盘坐标
    int row =(event->x()-m_margin+(m_cellSize/2))/m_cellSize;
    int col =(event->y()-m_margin+(m_cellSize/2))/m_cellSize;
    // qDebug()<<"row:"<<row;
    // qDebug()<<"col:"<<col;
    if(row>= 0&&row<m_boardSize&&col>=0&&col<m_boardSize){
        if(m_hoverCol!=row||m_hoverRow!=col){
            m_hoverCol=col;
            m_hoverRow =row;
            m_hoverFlag = true;
            //手动触发绘画事件
            update();
        }
    }else{
        m_hoverFlag =false;
        //不在棋盘时清空悬停点坐标
        m_hoverRow=m_hoverCol=-1;
        update();
    }
}

void QiPan::mousePressEvent(QMouseEvent *event) {
    if (m_gameover || (!m_myTurn && m_net))  // 联机模式必须轮到自己
        return;

    int row = (event->x() - m_margin + m_cellSize / 2) / m_cellSize;
    int col = (event->y() - m_margin + m_cellSize / 2) / m_cellSize;

    if (row < 0 || row >= m_boardSize || col < 0 || col >= m_boardSize)
        return;

    if (m_board[row][col] != EMPTY)
        return;

    // 设置当前角色
    if(m_isHost){
        ROLE myRole = m_isHost ? PLAYER : COMPUTER;
    };
    m_board[row][col] = currentRole;
    m_step.append({row, col, currentRole});
    update();
    // 联机模式：发送坐标、禁止继续落子
    if (m_net) {
        m_net->sendMove(row, col);
        m_myTurn = false;
    }

    // 判断胜负
    if (isCheckWin(row, col, currentRole)) {
        m_gameover = true;
        update();
        QString winner;
        if(m_mode==AI_MODE){
            winner= (currentRole == PLAYER) ? "玩家胜利" : "电脑胜利";
        };
        if(m_mode==NET_MODE){
            winner= (currentRole == PLAYER) ? "玩家1胜利" : "玩家2胜利";
        };
        if(m_mode==DUAL_MODE){
            winner= (currentRole == PLAYER) ? "玩家1胜利" : "玩家2胜利";
        };

        QMessageBox::information(this, "GAME OVER", winner);
        return;
    }

    // 切换角色
    currentRole = (currentRole == PLAYER) ? COMPUTER : PLAYER;
    emit currentRoleChange(currentRole);
    update();

    // AI 模式下由 AI 落子
    // QTimer::singleShot(300, this, [=]() {
    //     makeAIMove();  // 由 AI 落子（自动切换 currentRole）
    // });
    qDebug() << "[AI触发检查] m_useAi:" << m_useAi << ", currentRole:" << currentRole;
    if (m_useAi && currentRole == COMPUTER && !m_net && !m_gameover) {
        QTimer::singleShot(300, this, [=]() {
            makeAIMove();  // 由 AI 落子（自动切换 currentRole）
        });
    };
    emit gameOverOccurred();
}

//判断是否赢
bool QiPan::isCheckWin(int row, int col, ROLE role){
    //四个方向判断是否连起来
    int dierction[4][2]={
        {1,0},//水平方向
        {0,1},//竖直方向
        {1,-1},//左下右上
        {1,1},//左上右下
    };

    //遍历每一个方向
    for(int direct = 0;direct<4;direct++)
    {
        int cnt = 1;
        //向一个方向探测
        for(int idx = 1;idx<5;idx++){
            //下一个点的坐标
            int nextRow =row +dierction[direct][0]*idx;
            int nextCol =col +dierction[direct][1]*idx;

            if((nextRow>=0&&nextRow<m_boardSize)&&
                (nextCol>=0&&nextCol<m_boardSize)&&
                (m_board[nextRow][nextCol]==role)){
                cnt++;
            }else{
                break;
            }
        }
        //向相反方向探测
        for(int idx = 1;idx<5;idx++){
            //下一个点的坐标
            int nextRow =row-dierction[direct][0]*idx;
            int nextCol =col-dierction[direct][1]*idx;
            if((nextRow>=0&&nextRow<m_boardSize)&&
                (nextCol>=0&&nextCol<m_boardSize)&&
                (m_board[nextRow][nextCol]==role)){
                cnt++;
            }else{
                break;
            }
        }
        if(cnt>=5){
            return true;
        }
    }
    return false;
}

//重新开始
void QiPan::reStartGame()
{
    for(int i =0;i<m_boardSize;i++){
        for(int j = 0;j<m_boardSize;j++){
            m_board[i][j]=EMPTY;
        }
    }
    m_hoverCol=-1;
    m_hoverRow=-1;
    m_hoverFlag=false;
    currentRole =PLAYER;
    m_gameover=false;
    m_undoCount=3;
    m_step.clear();
    emit undoCountChange(m_undoCount);
    update();
}
//悔棋
void QiPan::undostep()
{
    if(m_gameover){
        return;
    };
    if(m_step.isEmpty()){
        return;
    };
    if(m_undoCount<=0){
        QMessageBox::information(this,"提示","次数已用净,不可悔棋");
    };
    if(m_undoCount>0)
    {
        step last=m_step.takeLast();
        m_board[last.row][last.col]=EMPTY;
        currentRole = (last.role == PLAYER) ? PLAYER : COMPUTER;;
        m_undoCount--;
        emit currentRoleChange(currentRole);//发出信号
        emit undoCountChange(m_undoCount);
        update();
    };
    if(m_undoCount<=0){
        m_undoCount=0;
    }

}

void QiPan::makeAIMove()
{
    if(m_gameover)return;
    bool hasComputer =false;
    for(const step &s :m_step){
        if(s.role==COMPUTER){
            hasComputer = true;
            break;
        }
    }
    if(!hasComputer){
        int center = m_boardSize/2;
        m_board[center][center]=COMPUTER;
        m_step.append({center,center,COMPUTER});
        currentRole = PLAYER;
        emit currentRoleChange(currentRole);
        update();
        return;
    }
    QVector<QVector<ROLE>> boardVec(m_boardSize,QVector<ROLE>(m_boardSize));
    for(int i =0;i<m_boardSize;i++){
        for(int j=0;j<m_boardSize;j++){
            boardVec[i][j]=m_board[i][j];
        }
    }
    QPair<int,int> best= ai->findBestMove(boardVec);
    int row = best.first;
    int col = best.second;
    if(row==-1)return;
    m_board[row][col]= COMPUTER;
    m_step.append({row,col,COMPUTER});
    update();
    if(isCheckWin(row,col,COMPUTER)){
        m_gameover=true;
        QMessageBox::information(this,"游戏结束","电脑胜利");
        return;
    }
    currentRole = PLAYER;
    emit currentRoleChange(currentRole);
}

void QiPan::exportStepsToFile()
{
    QString filename = QFileDialog::getSaveFileName(this,"保存棋谱","棋谱文件(*.txt)");
    if(filename.isEmpty())return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::warning(this,"错误","无法保存文件");
            return;
    }
    QTextStream out(&file);
    out<<"五子棋棋谱";
    for(int i =0;i<m_step.size();++i){
        step s =m_step[i];
        QString who=(s.role==PLAYER)?"玩家":"电脑";
        out<<QString("第%1步：%2->(%3,%4)\n").arg(i+1).arg(who).arg(s.col).arg(s.col);
    }
    file.close();
    QMessageBox::information(this,"成功","棋谱保存到文件");

}

void QiPan::setNetManager(NetManager *net, bool isHost)
{
    m_net = net;
    m_isHost = isHost;
    m_myTurn = isHost;

    //绑定网络接收事件
    connect(net,&NetManager::moveReceived,this,[=](int row,int col){
        if(m_board[row][col]!=EMPTY)return;
        ROLE rivalRole = m_isHost?COMPUTER:PLAYER;
        m_board[row][col] =rivalRole;
        m_step.append({row,col,rivalRole});
        update();
        m_myTurn = true;
    });
}
void QiPan::setGameMode(GameMode mode){
    m_mode = mode;
    m_useAi = (mode ==AI_MODE);
    m_net=nullptr;
    m_myTurn = true;
}

void QiPan::setAiMode(bool enabled)
{
    m_useAi =enabled;
}

void QiPan::importStepsFromFiles()
{
    QString filename =QFileDialog::getOpenFileName(this,"导入棋谱","","棋谱文件 (*.txt)");
    if(filename.isEmpty())return;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::warning(this,"错误","无法打开文件");
        return;
    }
    QTextStream in(&file);
    QString line;
    m_step.clear();
    reStartGame();

    while(!in.atEnd()){
        line  =in.readLine();
        if(line.startsWith("第")){
            QRegularExpression rx("第\\d+步：(.+)->\\((\\d+),(\\d+)\\)");
            QRegularExpressionMatch match = rx.match(line);
            if(match.hasMatch()){
                QString who = match.captured(1);
                int row = match.captured(2).toInt();
                int col = match.captured(3).toInt();
                ROLE role =(who=="玩家")?PLAYER:COMPUTER;
                m_board[row][col] = role;
                m_step.append({row,col,role});
            }
        }
    }
}

void QiPan::saveGame()
{
    QString fileName = QFileDialog::getSaveFileName(this,"保存对局","","对局文件(*.json)");
    if(fileName.isEmpty())return;

    QJsonArray stepArray;
    for(const step &s:m_step){
        QJsonObject obj;
        obj["row"]=s.row;
        obj["col"]=s.col;
        obj["role"] = s.role;
        stepArray.append(obj);
    }
    QJsonObject root;
    root["steps"] =stepArray;
    root["currentRole"] = currentRole;
    root["gameover"]=m_gameover;
    root["undoCount"] = m_undoCount;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this,"错误","无法写入文件");
        return;
    }

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();

    QMessageBox::information(this,"成功","游戏已保存");
}

void QiPan::loadGame()
{
    QString fileName = QFileDialog::getOpenFileName(this,"加载对局","","对局文件(*.json)");
    if(fileName.isEmpty())return;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"错误","无法读取文件");
        return;
    }
    QByteArray data= file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject()){
        QMessageBox::warning(this,"错误","文件格式不正确");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray steps = root["steps"].toArray();

    reStartGame();

    for(const QJsonValue &val:steps){
        QJsonObject obj= val.toObject();
        int row =obj["row"].toInt();
        int col = obj["col"].toInt();
        int role = obj["role"].toInt();
        m_board[row][col]=(ROLE)role;
        m_step.append({row,col,(ROLE)role});

        currentRole = (ROLE)root["currentRole"].toInt();
        m_gameover = root["gameover"].toBool();
        m_undoCount = root["undoCount"].toInt();

        emit currentRoleChange(currentRole);
        emit undoCountChange(m_undoCount);
        update();
    }
    QMessageBox::information(this,"成功","游戏已加载");
}

