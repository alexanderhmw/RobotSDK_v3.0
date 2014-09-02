#include "configuration.h"

Configuration::Configuration(QWidget *parent)
	: QMainWindow(parent)
{
    ui.setupUi(this);
#ifdef Q_OS_LINUX
    vsversion="Linux";
#elif defined(Q_OS_WIN)
#ifdef _MSC_VER
    vsversion=QInputDialog::getItem(this,QString("Select Compiler Version and Platform"),QString("Compiler Version and Platform"),QStringList()<<"v110_x64"<<"v110_Win32"<<"v100_x64"<<"v100_Win32",-1);
	if(vsversion.size()==0)
	{
		exit(0);
	}
#else
    vsversion="MinGW";
#endif
	QString sdkdriver;
#endif
	ui.vsversion->setText(vsversion);

	QFile file(QString("%1_%2.xml").arg(ConfigurationFile).arg(vsversion));
	doc=new QDomDocument(QString("Configuration"));
	if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
	{	
		doc->appendChild(doc->createElement("Configuration"));
#ifdef Q_OS_WIN
		QFileInfoList driverlist=QDir::drives();
		int i,n=driverlist.size();
		QStringList drivers;
		for(i=0;i<n;i++)
		{
			drivers<<driverlist[i].path();
		}
		sdkdriver=QInputDialog::getItem(this,"Select Driver to Install RobotSDK","Driver: ",drivers,-1);
		if(sdkdriver.size()==0)
		{
			exit(0);
		}
#endif
	}
	else
	{
        QString errmsg;
        if(!doc->setContent(&file,&errmsg))
		{
			delete doc;
			file.close();
            QMessageBox::information(this,QString("Configuration Error: "),QString("%1").arg(errmsg));
			exit(0);
		}
		file.close();
	}
	QDomElement root=doc->documentElement();

	robotdepdir=new RegisterDirWidget(this,doc,RobotDep,QStringList()<<"Include"<<"Lib"<<"Bin",1);
#ifdef Q_OS_LINUX
#elif defined(Q_OS_WIN)
    this->addItem(root,RobotDep,"Include",0,"Qt");
    this->addItem(root,RobotDep,"Lib",0,"Qt");
    this->addItem(root,RobotDep,"Bin",1,"Qt");
    this->addItem(root,RobotDep,"Include",0,"Boost");
    this->addItem(root,RobotDep,"Lib",0,"Boost");
    this->addItem(root,RobotDep,"Bin",1,"Boost");
#endif
	robotdepdir->loadRegisterDir(root);
	ui.Panel->addTab(robotdepdir,RobotDep);

	robotsdkdir=new RegisterDirWidget(this,doc,RobotSDK,QStringList()<<"Kernel"<<"ModuleDev"<<"Module"<<"SharedLibrary"<<"Tools",0);
#ifdef Q_OS_LINUX
    this->addItem(root,RobotSDK,"Kernel",0,"Kernel","/opt/RobotSDK/Kernel",1);
    this->addItem(root,RobotSDK,"ModuleDev",0,"ModuleDev",QString("%1/SDK/RobotSDK/ModuleDev").arg(QString(qgetenv("HOME"))),1);
    this->addItem(root,RobotSDK,"Module",0,"Module","/opt/RobotSDK/Module/include",1);
    this->addItem(root,RobotSDK,"SharedLibrary",1,"SharedLibrary","/opt/RobotSDK/Module/SharedLibrary",1);
    this->addItem(root,RobotSDK,"Tools",1,"Tools","/opt/RobotSDK/Tools",1);
#elif defined(Q_OS_WIN)
    this->addItem(root,RobotSDK,"Kernel",0,"Kernel",QString("%1SDK/RobotSDK/Kernel").arg(sdkdriver));
    this->addItem(root,RobotSDK,"ModuleDev",0,"ModuleDev",QString("%1SDK/RobotSDK/ModuleDev").arg(sdkdriver));
    this->addItem(root,RobotSDK,"Module",0,"Module",QString("%1SDK/RobotSDK/Module/include").arg(sdkdriver));
    this->addItem(root,RobotSDK,"SharedLibrary",1,"SharedLibrary",QString("%1SDK/RobotSDK/Module/SharedLibrary").arg(sdkdriver));
    this->addItem(root,RobotSDK,"Tools",1,"Tools",QString("%1SDK/RobotSDK/Tools").arg(sdkdriver));
#endif
	robotsdkdir->loadRegisterDir(root);
	ui.Panel->addTab(robotsdkdir,RobotSDK);
}

Configuration::~Configuration()
{
	delete doc;
	doc=new QDomDocument(QString("Configuration"));

	QDomElement root=doc->appendChild(doc->createElement("Configuration")).toElement();
	robotsdkdir->saveRegisterDir(root);
	robotdepdir->saveRegisterDir(root);

	QFile file(QString("%1_%2.xml").arg(ConfigurationFile).arg(vsversion));
	if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
	{
		QMessageBox::information(this,QString("Configuration Error"),QString("Cannot Write %1").arg(ConfigurationFile));
		exit(0);
	}
	QTextStream textstream;
	textstream.setDevice(&file);
	doc->save(textstream,2);
	file.close();
	delete doc;
}

bool Configuration::addItem(QDomElement root, QString qstrRegisterName, QString qstrRegisterValue, bool addPath, QString qstrRegisterTag, QString qstrRegisterDir, bool force)
{
    QDomElement node=root.firstChildElement(qstrRegisterName);
    if(node.isNull())
    {
        node=root.appendChild(doc->createElement(qstrRegisterName)).toElement();
    }
    QDomElement tmpnode=node.firstChildElement(QString("%1_%2").arg(qstrRegisterName).arg(qstrRegisterValue));
    if(tmpnode.isNull())
    {
        node=node.appendChild(doc->createElement(QString("%1_%2").arg(qstrRegisterName).arg(qstrRegisterValue))).toElement();
    }
    else
    {
        node=tmpnode;
    }
    node.setAttribute(PATHATTR,uint(addPath));
    tmpnode=node.firstChildElement(qstrRegisterTag);
    if(tmpnode.isNull())
    {
        node=node.appendChild(doc->createElement(qstrRegisterTag)).toElement();
        node.appendChild(doc->createTextNode(qstrRegisterDir));
        QDir dir(qstrRegisterDir);
        if(!qstrRegisterDir.isEmpty()&&!dir.exists())
        {
            dir.mkpath(qstrRegisterDir);
        }
    }
    else if(force)
    {
        node=tmpnode;
        if(node.hasChildNodes())
        {
            QDomNode tmp=node.firstChild();
            while(!tmp.isNull()&&tmp.nodeType()!=QDomNode::TextNode)
            {
                tmp=tmp.nextSibling();
            }
            if(tmp.isNull())
            {
                node.appendChild(doc->createTextNode(qstrRegisterDir));
            }
            else
            {
                tmp.toText().setNodeValue(qstrRegisterDir);
            }
        }
        else
        {
            node.appendChild(doc->createTextNode(qstrRegisterDir));
        }
        QDir dir(qstrRegisterDir);
        if(!qstrRegisterDir.isEmpty()&&!dir.exists())
        {
            dir.mkpath(qstrRegisterDir);
        }
    }
    return 1;
}
