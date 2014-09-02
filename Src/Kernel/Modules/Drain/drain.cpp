#include "drain.h"

Drain::Drain(QString qstrSharedLibrary, QString qstrNodeType, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName)
	: Node(qstrSharedLibrary,qstrNodeType,qstrNodeClass,qstrNodeName,qstrConfigName)
{
	if(outputports.size()>0)
	{
		QMessageBox::information(NULL,QString("Node Error"),QString("%1_%2_%3: Number of output ports >0").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

DrainMono::DrainMono(QString qstrSharedLibrary, QString qstrNodeType, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: Drain(qstrSharedLibrary,qstrNodeType,qstrNodeClass,qstrNodeName,qstrConfigName)
{
	LoadCheckFptr(sharedlibrary,getMonoDrainDataSizeFptr,getMonoDrainDataSize,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,processMonoDrainDataFptr,processMonoDrainData,qstrFuncEx,nodetype,nodeclass);
	if(inputports.size()!=1)
	{
		QMessageBox::information(NULL,QString("Node Error"),QString("%1_%2_%3: Number of input ports !=1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

DrainMono::DrainMono(QString qstrSharedLibrary, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: Drain(qstrSharedLibrary,QString("DrainMono"),qstrNodeClass,qstrNodeName,qstrConfigName)
{
	LoadCheckFptr(sharedlibrary,getMonoDrainDataSizeFptr,getMonoDrainDataSize,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,processMonoDrainDataFptr,processMonoDrainData,qstrFuncEx,nodetype,nodeclass);
	if(inputports.size()!=1)
	{
		QMessageBox::information(NULL,QString("Node Error"),QString("%1_%2_%3: Number of input ports !=1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

void DrainMono::processDrainDataSlot()
{
	if(openflag)
	{
		nodeTriggerTime(NodeTriggerStart);
		int inputdatasize;
		getMonoDrainDataSize(paramsptr.get(),varsptr.get(),inputdatasize);
        QVector<boost::shared_ptr<void> >boostparams=inputports[0]->grabInputParams(inputdatasize);
        QVector<boost::shared_ptr<void> >boostdata=inputports[0]->grabInputData(inputdatasize);
		if(processMonoDrainData(paramsptr.get(),varsptr.get(),convertBoostData(boostparams),convertBoostData(boostdata)))
		{
			emit processDrainDataSignal();
			nodeTriggerTime(NodeTriggerEnd);
		}
		else
		{
			emit processDrainDataErrorSignal();
			nodeTriggerTime(NodeTriggerError);
		}
	}
}

DrainMulti::DrainMulti(QString qstrSharedLibrary, QString qstrNodeType, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: Drain(qstrSharedLibrary,qstrNodeType,qstrNodeClass,qstrNodeName,qstrConfigName)
{
	LoadCheckFptr(sharedlibrary,getMultiDrainDataSizeFptr,getMultiDrainDataSize,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,processMultiDrainDataFptr,processMultiDrainData,qstrFuncEx,nodetype,nodeclass);
	if(inputports.size()<=1)
	{
		QMessageBox::information(NULL,QString("DrainMulti Node Error"),QString("DrainMulti %1_%2_%3: Number of input ports <=1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

DrainMulti::DrainMulti(QString qstrSharedLibrary, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: Drain(qstrSharedLibrary,QString("DrainMulti"),qstrNodeClass,qstrNodeName,qstrConfigName)
{
	LoadCheckFptr(sharedlibrary,getMultiDrainDataSizeFptr,getMultiDrainDataSize,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,processMultiDrainDataFptr,processMultiDrainData,qstrFuncEx,nodetype,nodeclass);
	if(inputports.size()<=1)
	{
		QMessageBox::information(NULL,QString("DrainMulti Node Error"),QString("DrainMulti %1_%2_%3: Number of input ports <=1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

void DrainMulti::processDrainDataSlot()
{
	if(openflag)
	{
		nodeTriggerTime(NodeTriggerStart);
		
		int i,n=inputports.size();
        QVector<QVector<boost::shared_ptr<void> > > boostparams(n);
        QVector<QVector<void *> > drainparams(n);
        QVector<QVector<boost::shared_ptr<void> > > boostdata(n);
        QVector<QVector<void *> > draindata(n);
		QList<int> multidatasize;
		getMultiDrainDataSize(paramsptr.get(),varsptr.get(),multidatasize);
		int m=multidatasize.size();
		for(i=0;i<n;i++)
		{			
			if(i<m)
			{
				boostparams[i]=inputports[i]->grabInputParams(multidatasize[i]);
				boostdata[i]=inputports[i]->grabInputData(multidatasize[i]);
			}
			else
			{
				boostparams[i]=inputports[i]->grabInputParams(-1);
				boostdata[i]=inputports[i]->grabInputData(-1);
			}
			drainparams[i]=convertBoostData(boostparams[i]);
			draindata[i]=convertBoostData(boostdata[i]);
		}
		if(processMultiDrainData(paramsptr.get(),varsptr.get(),drainparams,draindata))
		{
			emit processDrainDataSignal();
			nodeTriggerTime(NodeTriggerEnd);
		}
		else
		{
			emit processDrainDataErrorSignal();
			nodeTriggerTime(NodeTriggerError);
		}
	}
}
