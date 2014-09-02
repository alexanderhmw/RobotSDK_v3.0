#include "edge.h"

Edge::Edge()
{
    QVBoxLayout * layout=new QVBoxLayout();
    QHBoxLayout * hlayout=new QHBoxLayout();
    hlayout->addWidget(new QLabel("Timer Speed (ms)"));
    hlayout->addWidget(&timerspeed);
    playpause.setText("Play");
    hlayout->addWidget(&playpause);
    hlayout->addStretch();
    hlayout->addWidget(new QLabel("Time Range (ms)"));
    hlayout->addWidget(&timerangeinput);
    hlayout->addWidget(new QLabel("Time Interval (ms)"));
    hlayout->addWidget(&timeintervalinput);
    QPushButton * set=new QPushButton("Set Time Line");
    hlayout->addWidget(set);
    QPushButton * moveup=new QPushButton("Move Up");
    hlayout->addWidget(moveup);
    QPushButton * movedown=new QPushButton("Move Down");
    hlayout->addWidget(movedown);

    layout->addLayout(hlayout);
    layout->addWidget(&panel);
    this->setLayout(layout);

    timerspeed.setText("50");
    timerange=1000;
    timeinterval=100;
    timerangeinput.setText(QString("%1").arg(timerange));
    timeintervalinput.setText(QString("%1").arg(timeinterval));
    panel.setSortingEnabled(0);
    panel.setColumnCount(2);
    panel.setHorizontalHeaderLabels(QStringList()<<"Trigger Log"<<"Trigger View");

    moveup->setEnabled(0);
    movedown->setEnabled(0);

    bool flag=1;
    flag&=bool(connect(set,SIGNAL(clicked()),this,SLOT(setTimeLineSlot())));
    flag&=bool(connect(moveup,SIGNAL(clicked()),this,SLOT(moveUpSlot())));
    flag&=bool(connect(movedown,SIGNAL(clicked()),this,SLOT(moveDownSlot())));
    flag&=bool(connect(&timer,SIGNAL(timeout()),this,SLOT(drawSlot())));
    flag&=bool(connect(&playpause,SIGNAL(clicked()),this,SLOT(playPauseTimerSlot())));
}

Edge::~Edge()
{
    timer.stop();
    clear();
}

bool Edge::connectNodes(Node * outputNode, Node * inputNode)
{
    QVector<QString> outputnodesname=outputNode->getOutputNodesName();
    QVector<QString> inputnodesname=inputNode->getInputNodesName();
    QString outputnodename=outputNode->getNodeName();
    QString inputnodename=inputNode->getNodeName();
    int i,n;
    n=outputnodesname.size();
    int outputportindex=-1;
    for(i=0;i<n;i++)
    {
        QStringList nodenames=outputnodesname[i].split(";",QString::SkipEmptyParts);
        int j,m=nodenames.size();
        for(j=0;j<m;j++)
        {
            if(nodenames[j]==inputnodename)
            {
                outputportindex=i;
                break;
            }
        }
    }
    n=inputnodesname.size();
    int inputportindex=-1;
    for(i=0;i<n;i++)
    {
        QStringList nodenames=inputnodesname[i].split(";",QString::SkipEmptyParts);
        int j,m=nodenames.size();
        for(j=0;j<m;j++)
        {
            if(nodenames[j]==outputnodename)
            {
                inputportindex=i;
                break;
            }
        }
    }
    if(outputportindex<0||inputportindex<0)
    {
        return 1;
    }
    return bool(connect(outputNode->getOutputPort(outputportindex),SIGNAL(outputDataSignal(boost::shared_ptr<void>, boost::shared_ptr<void>))
        ,inputNode->getInputPort(inputportindex),SLOT(inputDataSlot(boost::shared_ptr<void>,  boost::shared_ptr<void>)),Qt::BlockingQueuedConnection));
}

bool Edge::disconnectNodes(Node * inputNode, Node * outputNode)
{
    QVector<QString> outputnodesname=outputNode->getOutputNodesName();
    QVector<QString> inputnodesname=inputNode->getInputNodesName();
    QString outputnodename=outputNode->getNodeName();
    QString inputnodename=inputNode->getNodeName();
    int i,n;
    n=outputnodesname.size();
    int outputportindex=-1;
    for(i=0;i<n;i++)
    {
        if(outputnodesname[i]==inputnodename)
        {
            outputportindex=i;
            break;
        }
    }
    n=inputnodesname.size();
    int inputportindex=-1;
    for(i=0;i<n;i++)
    {
        if(inputnodesname[i]==outputnodename)
        {
            inputportindex=i;
            break;
        }
    }
    if(outputportindex<0||inputportindex<0)
    {
        return 1;
    }
    return bool(disconnect(outputNode->getOutputPort(outputportindex),SIGNAL(outputDataSignal(boost::shared_ptr<void>, boost::shared_ptr<void>))
        ,inputNode->getInputPort(inputportindex),SLOT(inputDataSlot(boost::shared_ptr<void>,  boost::shared_ptr<void>))));
}

void Edge::addNode(Node * node, bool gotoThread, bool needMonitor)
{
    int n=nodepool.size();
    nodepool.insert(node);
    int m=nodepool.size();
    if(m==n+1)
    {
        bool flag=1;
        if(gotoThread)
        {
            QThread * thread=new QThread();
            node->moveToThread(thread);
            thread->start();
            threads.push_back(thread);
            flag&=bool(connect(this,SIGNAL(openAllNodesSignal()),node,SLOT(openNodeSlot()),Qt::BlockingQueuedConnection));
            //flag&=bool(connect(this,SIGNAL(closeAllNodesSignal()),node,SLOT(closeNodeSlot()),Qt::BlockingQueuedConnection));
            //flag&=bool(connect(this,SIGNAL(openAllNodesSignal()),node,SLOT(openNodeSlot())));
            flag&=bool(connect(this,SIGNAL(closeAllNodesSignal()),node,SLOT(closeNodeSlot())));
        }
        else
        {
            flag&=bool(connect(this,SIGNAL(openAllNodesSignal()),node,SLOT(openNodeSlot())));
            flag&=bool(connect(this,SIGNAL(closeAllNodesSignal()),node,SLOT(closeNodeSlot())));
        }
        if(needMonitor)//&&gotoThread)
        {
            TriggerLog * triggerlog=new TriggerLog(this,node,gotoThread);
            TriggerView * triggerview=new TriggerView(this,node,timerange,timeinterval,gotoThread);
            int row=panel.rowCount();
            panel.insertRow(row);
            //panel.setVerticalHeaderItem(row,new QTableWidgetItem(QString("%1_%2_%3").arg(node->getNodeType()).arg(node->getNodeClass()).arg(node->getNodeName())));
            panel.setCellWidget(row,0,triggerlog);
            panel.setCellWidget(row,1,triggerview);
            panel.resizeColumnsToContents();
            panel.resizeRowsToContents();
        }
    }
}

void Edge::clear()
{
    bool flag=1;
    int i,n;

    flag&=disconnectAll();
    n=panel.rowCount();
    for(i=n-1;i>=0;i--)
    {
        /*panel.setCellWidget(i,0,NULL);
        panel.setCellWidget(i,1,NULL);*/
        TriggerLog * triggerlog=(TriggerLog *)panel.cellWidget(i,0);
        TriggerView * triggerview=(TriggerView *)panel.cellWidget(i,1);
        panel.removeCellWidget(i,0);
        panel.removeCellWidget(i,1);
        delete triggerlog;
        delete triggerview;
        panel.removeRow(i);
    }

    emit closeAllNodesSignal();

    n=threads.size();
    for(i=0;i<n;i++)
    {
        threads[i]->exit();
        threads[i]->wait();
        threads[i]->deleteLater();
    }
    threads.clear();

    QList<Node *> nodes;
    nodes=QList<Node *>::fromSet(nodepool);
    nodepool.clear();
    n=nodes.size();
    for(i=0;i<n;i++)
    {
        flag&=bool(disconnect(this,SIGNAL(openAllNodesSignal()),nodes.at(i),SLOT(openNodeSlot())));
        flag&=bool(disconnect(this,SIGNAL(closeAllNodesSignal()),nodes.at(i),SLOT(closeNodeSlot())));
        nodes[i]->deleteLater();
    }
}

bool Edge::connectAll()
{
    QList<Node *> outputnodes=nodepool.toList();
    QList<Node *> inputnodes=nodepool.toList();
    int i,j,n=nodepool.size();
    bool flag=1;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            flag&=connectNodes(outputnodes[i],inputnodes[j]);
        }
    }
    return flag;
}

bool Edge::disconnectAll()
{
    QList<Node *> outputnodes=nodepool.toList();
    QList<Node *> inputnodes=nodepool.toList();
    int i,j,n=nodepool.size();
    bool flag=1;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            if(i!=j)
            {
                flag&=disconnectNodes(outputnodes[i],inputnodes[j]);
            }
        }
    }
    return flag;
}

void Edge::openAllNodesSlot()
{
    emit openAllNodesSignal();
}

void Edge::closeAllNodesSlot()
{
    emit closeAllNodesSignal();
}

void Edge::playPauseTimerSlot()
{
    if(playpause.text()==QString("Play"))
    {
        int speed=timerspeed.text().toInt();
        if(speed>0)
        {
            playpause.setText("Pause");
            timer.start(speed);
        }
    }
    else
    {
        playpause.setText("Play");
        timer.stop();
    }
}

void Edge::setTimeLineSlot()
{
    int i,n=panel.rowCount();
    timerange=timerangeinput.text().toInt();
    timeinterval=timeintervalinput.text().toInt();
    for(i=0;i<n;i++)
    {
        TriggerView * triggerview=(TriggerView *)(panel.cellWidget(i,1));
        triggerview->setTimeLine(timerange,timeinterval);
    }
}

void Edge::moveUpSlot()
{
    int rowid=panel.currentRow();
    int columnid=panel.currentColumn();
    if(rowid>0)
    {
        TriggerLog * triggerlog=(TriggerLog *)(panel.cellWidget(rowid,0));
        TriggerView * triggerview=(TriggerView *)(panel.cellWidget(rowid,1));
        TriggerLog * uppertriggerlog=(TriggerLog *)(panel.cellWidget(rowid-1,0));
        TriggerView * uppertriggerview=(TriggerView *)(panel.cellWidget(rowid-1,1));
        panel.removeCellWidget(rowid,0);
        panel.removeCellWidget(rowid,1);
        panel.removeCellWidget(rowid-1,0);
        panel.removeCellWidget(rowid-1,1);
        panel.setCellWidget(rowid-1,0,triggerlog);
        panel.setCellWidget(rowid-1,1,triggerview);
        panel.setCellWidget(rowid,0,uppertriggerlog);
        panel.setCellWidget(rowid,1,uppertriggerview);
        panel.setCurrentCell(rowid-1,columnid);
    }
}

void Edge::moveDownSlot()
{
    int rowid=panel.currentRow();
    int columnid=panel.currentColumn();
    if(rowid<panel.rowCount()-1)
    {
        TriggerLog * triggerlog=(TriggerLog *)(panel.cellWidget(rowid,0));
        TriggerView * triggerview=(TriggerView *)(panel.cellWidget(rowid,1));
        TriggerLog * lowertriggerlog=(TriggerLog *)(panel.cellWidget(rowid+1,0));
        TriggerView * lowertriggerview=(TriggerView *)(panel.cellWidget(rowid+1,1));
        panel.removeCellWidget(rowid,0);
        panel.removeCellWidget(rowid,1);
        panel.removeCellWidget(rowid+1,0);
        panel.removeCellWidget(rowid+1,1);
        panel.setCellWidget(rowid+1,0,triggerlog);
        panel.setCellWidget(rowid+1,1,triggerview);
        panel.setCellWidget(rowid,0,lowertriggerlog);
        panel.setCellWidget(rowid,1,lowertriggerview);
        panel.setCurrentCell(rowid+1,columnid);
    }
}

void Edge::drawSlot()
{
    emit drawSignal(QDateTime::currentDateTime());
}
