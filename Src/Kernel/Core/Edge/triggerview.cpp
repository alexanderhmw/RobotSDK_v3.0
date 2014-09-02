#include "triggerview.h"

TriggerView::TriggerView(QWidget *parent, Node * node, int timeRange, int timeInterval, bool gotoThread)
    : QWidget(parent)
{
    backupnode=node;
    timerange=timeRange;
    timeinterval=timeInterval;

    nodetag=QString("%1_%2_%3").arg(node->getNodeType()).arg(node->getNodeClass()).arg(node->getNodeName());
    QVBoxLayout * layout=new QVBoxLayout();
    layout->addWidget(&pulse);
    pulse.setFixedWidth(timerange);
    this->setLayout(layout);

    bool flag=1;
    if(gotoThread)
    {
        flag&=bool(connect(node,SIGNAL(nodeTriggerTimeSignal(QDateTime, Node::NodeTriggerState)),this,SLOT(nodeTriggerTimeSlot(QDateTime, Node::NodeTriggerState)),Qt::BlockingQueuedConnection));
        //flag&=bool(connect(node,SIGNAL(nodeTriggerTimeSignal(QDateTime, Node::NodeTriggerState)),this,SLOT(nodeTriggerTimeSlot(QDateTime, Node::NodeTriggerState))));
    }
    else
    {
        flag&=bool(connect(node,SIGNAL(nodeTriggerTimeSignal(QDateTime, Node::NodeTriggerState)),this,SLOT(nodeTriggerTimeSlot(QDateTime, Node::NodeTriggerState))));
    }
    flag&=bool(connect(parent,SIGNAL(drawSignal(QDateTime)),this,SLOT(drawSlot(QDateTime))));
}

TriggerView::~TriggerView()
{
    bool flag=1;
    flag&=bool(disconnect(backupnode,SIGNAL(nodeTriggerTimeSignal(QDateTime, Node::NodeTriggerState)),this,SLOT(nodeTriggerTimeSlot(QDateTime, Node::NodeTriggerState))));
    flag&=bool(disconnect(backupnode,SIGNAL(drawSignal(QDateTime)),this,SLOT(drawSlot(QDateTime))));
}

void TriggerView::setTimeLine(int timeRange, int timeInterval)
{
    timerange=timeRange;
    timeinterval=timeInterval;
    pulse.setFixedWidth(timerange);
}

void TriggerView::nodeTriggerTimeSlot(QDateTime curDateTime, Node::NodeTriggerState nodeTriggerState)
{
    NodeTriggerPoint nodetriggerpoint;
    nodetriggerpoint.datetime=curDateTime;
    nodetriggerpoint.triggerstate=nodeTriggerState;
    triggerlist.push_back(nodetriggerpoint);
    if(triggerlist.size()>timerange)
    {
        triggerlist.pop_front();
    }
}


void TriggerView::drawSlot(QDateTime curDateTime)
{
    QRect rect=pulse.geometry();
    if(rect.width()==0||timeinterval<=0||timerange<=0)
    {
        return;
    }
    int width=rect.width();
    int height=rect.height();
    QFontMetrics fm=pulse.fontMetrics();
    QDateTime ancientDateTime=curDateTime.addMSecs(-timerange);
    Node::NodeTriggerState laststate=Node::UnknownSate;
    while(!triggerlist.isEmpty())
    {
        if(triggerlist.front().datetime>=ancientDateTime)
        {
            break;
        }
        else
        {
            laststate=triggerlist.front().triggerstate;
            triggerlist.pop_front();
        }
    }
    int i,n;
    QImage image(rect.width(),rect.height(),QImage::Format_ARGB32);
    image.fill(32);
    QPainter painter;
    painter.begin(&image);

    n=timerange/timeinterval;
    painter.setPen(QColor(0,0,0,128));
    painter.drawText(0,fm.height(),curDateTime.toString("yyyy-MM-dd HH:mm:ss:zzz"));
    painter.drawLine(0,fm.height(),width,fm.height());
    painter.drawLine(0,height-fm.height(),width,height-fm.height());
    for(i=0;i<=n;i++)
    {
        QString timestamp=QString("%1ms").arg(i*(timeinterval));
        painter.drawLine(i*timeinterval,fm.height(),i*timeinterval,height-fm.height());
        painter.drawText(i*timeinterval+0.5,height,timestamp);
    }

    QPoint prev;
    prev.setX(width);
    switch(laststate)
    {
    case Node::UnknownSate:
        prev.setY(height-fm.height()-MARGIN);
        painter.setPen(QColor(0,0,0,255));
        break;
    case Node::NodeTriggerStart:
        prev.setY(fm.height()+MARGIN);
        painter.setPen(QColor(0,0,255,255));
        break;
    case Node::NodeTriggerEnd:
        prev.setY(height-fm.height()-MARGIN);
        painter.setPen(QColor(0,0,255,255));
        break;
    case Node::NodeTriggerError:
        prev.setY(height-fm.height()-MARGIN);
        painter.setPen(QColor(255,0,0,255));
        break;
    }
    QPoint next;
    n=triggerlist.size();
    for(i=0;i<n;i++)
    {
        int nx=triggerlist.at(i).datetime.msecsTo(curDateTime);
        next.setX(nx);
        next.setY(prev.y());
        painter.drawLine(prev,next);
        prev.setX(next.x());
        laststate=triggerlist.at(i).triggerstate;
        switch(laststate)
        {
        case Node::UnknownSate:
            prev.setY(height-fm.height()-MARGIN);
            painter.setPen(QColor(0,0,0,196));
            break;
        case Node::NodeTriggerStart:
            prev.setY(fm.height()+MARGIN);
            painter.setPen(QColor(0,0,255,255));
            break;
        case Node::NodeTriggerEnd:
            prev.setY(height-fm.height()-MARGIN);
            painter.setPen(QColor(0,0,255,255));
            break;
        case Node::NodeTriggerError:
            prev.setY(height-fm.height()-MARGIN);
            painter.setPen(QColor(255,0,0,255));
            break;
        }
        painter.drawLine(prev,next);
    }
    next.setX(0);
    next.setY(prev.y());
    painter.drawLine(prev,next);

    painter.end();

    pulse.clear();
    pulse.setPixmap(QPixmap::fromImage(image));
}
