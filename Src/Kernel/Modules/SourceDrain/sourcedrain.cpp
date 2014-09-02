#include "sourcedrain.h"

SourceDrain::SourceDrain(QString qstrSharedLibrary, QString qstrNodeType, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: Node(qstrSharedLibrary,qstrNodeType,qstrNodeClass,qstrNodeName,qstrConfigName)
{
	LoadCheckFptr(sharedlibrary,initializeOutputDataFptr,initializeOutputData,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,generateSourceDataFptr,generateSourceData,qstrFuncEx,nodetype,nodeclass);
	if(outputports.size()<1)
	{
		QMessageBox::information(NULL,QString("SourceDrainMono Node Error"),QString("SourceDrainMono %1_%2_%3: Number of output ports <1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

void SourceDrain::generateSourceDataSlot()
{
	if(openflag)
	{
		nodeTriggerTime(NodeTriggerStart);
		boost::shared_ptr<void> outputdata;
		initializeOutputData(paramsptr.get(),varsptr.get(),outputdata);
		int outputportindex;
		QTime timestamp;
		generateSourceData(paramsptr.get(),varsptr.get(),outputdata.get(),outputportindex,timestamp);
		if(outputportindex>=0)
		{
			if(outputportindex>=outputports.size())
			{
				int i,n=outputports.size();
				for(i=0;i<n;i++)
				{
					outputports[i]->outputData(paramsptr,outputdata);
				}
			}
			else
			{
				outputports[outputportindex]->outputData(paramsptr,outputdata);
			}
			emit generateSourceDataSignal();
			nodeTriggerTime(NodeTriggerEnd);
		}
		else
		{
			emit generateSourceDataErrorSignal();
			nodeTriggerTime(NodeTriggerError);
		}
	}
}

SourceDrainMono::SourceDrainMono(QString qstrSharedLibrary, QString qstrNodeType, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: SourceDrain(qstrSharedLibrary,qstrNodeType,qstrNodeClass,qstrNodeName,qstrConfigName,qstrFuncEx)
{	
	LoadCheckFptr(sharedlibrary,getMonoDrainDataSizeFptr,getMonoDrainDataSize,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,processMonoDrainDataFptr,processMonoDrainData,qstrFuncEx,nodetype,nodeclass);
	if(inputports.size()!=1)
	{
		QMessageBox::information(NULL,QString("SourceDrainMono Node Error"),QString("SourceDrainMono %1_%2_%3: Number of input ports !=1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

SourceDrainMono::SourceDrainMono(QString qstrSharedLibrary, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: SourceDrain(qstrSharedLibrary,QString("SourceDrainMono"),qstrNodeClass,qstrNodeName,qstrConfigName,qstrFuncEx)
{
	LoadCheckFptr(sharedlibrary,getMonoDrainDataSizeFptr,getMonoDrainDataSize,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,processMonoDrainDataFptr,processMonoDrainData,qstrFuncEx,nodetype,nodeclass);
	if(inputports.size()!=1)
	{
		QMessageBox::information(NULL,QString("SourceDrainMono Node Error"),QString("SourceDrainMono %1_%2_%3: Number of input ports !=1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

void SourceDrainMono::processDrainDataSlot()
{
	if(openflag)
	{
		nodeTriggerTime(NodeTriggerStart);		
		int monodatasize;
		getMonoDrainDataSize(paramsptr.get(),varsptr.get(),monodatasize);
        QVector<boost::shared_ptr<void> > boostparams=inputports[0]->grabInputParams(monodatasize);
        QVector<boost::shared_ptr<void> > boostdata=inputports[0]->grabInputData(monodatasize);
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

SourceDrainMulti::SourceDrainMulti(QString qstrSharedLibrary, QString qstrNodeType, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: SourceDrain(qstrSharedLibrary,qstrNodeType,qstrNodeClass,qstrNodeName,qstrConfigName,qstrFuncEx)
{
	LoadCheckFptr(sharedlibrary,getMultiDrainDataSizeFptr,getMultiDrainDataSize,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,processMultiDrainDataFptr,processMultiDrainData,qstrFuncEx,nodetype,nodeclass);
	if(inputports.size()<=1)
	{
		QMessageBox::information(NULL,QString("DrainMulti Node Error"),QString("DrainMulti %1_%2_%3: Number of input ports <=1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

SourceDrainMulti::SourceDrainMulti(QString qstrSharedLibrary, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
	: SourceDrain(qstrSharedLibrary,QString("SourceDrainMulti"),qstrNodeClass,qstrNodeName,qstrConfigName,qstrFuncEx)
{
	LoadCheckFptr(sharedlibrary,getMultiDrainDataSizeFptr,getMultiDrainDataSize,nodetype,nodeclass);
	LoadCheckExFptr(sharedlibrary,processMultiDrainDataFptr,processMultiDrainData,qstrFuncEx,nodetype,nodeclass);
	if(inputports.size()<=1)
	{
		QMessageBox::information(NULL,QString("DrainMulti Node Error"),QString("DrainMulti %1_%2_%3: Number of input ports <=1").arg(nodetype).arg(nodeclass).arg(nodename));
		exit(0);
	}
}

void SourceDrainMulti::processDrainDataSlot()
{
	if(openflag)
	{
		nodeTriggerTime(NodeTriggerStart);
		int i,n=inputports.size();
        QVector<QVector<boost::shared_ptr<void> > > boostparams(n);
        QVector<QVector<boost::shared_ptr<void> > > boostdata(n);
        QVector<QVector<void *> > inputparams(n);
        QVector<QVector<void *> > inputdata(n);
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
			inputparams[i]=convertBoostData(boostparams[i]);
			inputdata[i]=convertBoostData(boostdata[i]);
		}
		if(processMultiDrainData(paramsptr.get(),varsptr.get(),inputparams,inputdata))
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
