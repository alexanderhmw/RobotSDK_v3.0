#ifndef SCANINTERFACEFUNCTION_H
#define SCANINTERFACEFUNCTION_H

#include <QWidget>
#include "ui_scaninterfacefunction.h"

#include<qdom.h>
#include<qtextstream.h>
#include<qvector.h>
#include<qlist.h>
#include<qstring.h>
#include<qfile.h>
#include<qfiledialog.h>
#include<stdlib.h>
#include<qmessagebox.h>
#include<qdir.h>
#include<qtreewidget.h>
#include<qlistwidget.h>
#include<qplaintextedit.h>
#include<qtablewidget.h>
#include<qinputdialog.h>
#include<qdatetime.h>

#define INTERFACEFUNCTIONS "Interface_Functions"
#define ROBOTSDKGLOBAL "RobotSDK_Global"
#ifdef Q_OS_LINUX
    #define ROBOTSDKKERNEL "/opt/RobotSDK/Kernel/include"
    #define ROBOTSDKMODULEDEV QString("%1/SDK/RobotSDK/ModuleDev").arg(QString(qgetenv("HOME")))
    #define ROBOTSDKMODULE "/opt/RobotSDK/Module"
    #define ROBOTSDKSHAREDLIBRARY "/opt/RobotSDK/SharedLibrary"
#elif defined(Q_OS_WIN)
    #define ROBOTSDKKERNEL "RobotSDK_Kernel"
    #define ROBOTSDKMODULEDEV "RobotSDK_ModuleDev"
    #define ROBOTSDKMODULE "RobotSDK_Module"
    #define ROBOTSDKSHAREDLIBRARY "RobotSDK_SharedLibrary"
#endif
#define CREATERULE "CreateRule"
#define EDITFOLDER "Edit"
#define NOEDITFOLDER "NoEdit"

class InterfaceFunctionNode
{
public:
	QVector<QString> comment;
	QString returnvalue;
	QString functionname;
	QVector<QString> parametertypes;
	QVector<QString> parameternames;
public:
	bool scan(QTextStream & textstream, QString & classLine);
	void store(QDomDocument * doc, QDomElement & classRoot);
	void load(QDomElement & interfaceRoot);
};

class ClassNode
{
public:
	QString filename;
	QString classname;
	QString parentclass;
	QVector<InterfaceFunctionNode> interfacefunctions;
public:
	bool scan(QTextStream & textStream,QString fileName);
	void store(QDomDocument * doc, QDomElement & root);
	void load(QDomElement & classRoot);
};

class ScanInterfaceFunction : public QWidget
{
	Q_OBJECT
public:
	ScanInterfaceFunction(QWidget *parent = 0);
	~ScanInterfaceFunction();
private:
	Ui::ScanInterfaceFunction ui;
protected:
	QVector<ClassNode> classnodes;
public slots:
	void scanKernelDirSlot();
	void showInterfaceFunctionsSlot();
	void showDetailInformationSlot();
	void browseParamsHeaderSlot();
	void clearParamsHeaderSlot();
	void browseOutputDataHeaderSlot();
	void clearOutputDataHeaderSlot();
	void browseVarsHeaderSlot();
	void clearVarsHeaderSlot();
	void addInputDataParamsHeadersSlot();
	void browseInputDataParamsHeadersSlot();
	void deleteInputDataParamsHeadersSlot();
    void clearInputDataParamsHeadersSlot();
	void addInputPortsSizeSlot();
	void deleteInputPortsSizeSlot();
    void clearInputPortsSizeSlot();
	void processDoubleClickInputParamsDataSlot(int row, int column);
	void processDoubleClickInputPortSizeSlot(int row, int column);
	void addExFuncSlot();
	void deleteExFuncSlot();
	void clearExFuncsSlot();
	void browseOutputDirSlot();
	void createLibraryFilesSlot();
    void browseNewOutputDirSlot();
    void createNewLibraryFilesSlot();
protected:
	QDomDocument * doc;
	QDomElement root;
	QDomDocument * ruledoc;
	QDomElement ruleroot;
protected:
	void scan(QString directory);
	void checkRules(ClassNode & classNode);
	QDomElement checkTag(QDomElement & tmproot, QString tag, QString defaultValue=QString());
    void showRule();
    void storeRule();
    bool createRule();
protected:
	QString nodetypename;
	QVector<InterfaceFunctionNode> interfacefunctions;
	QString nodeclassname;
	QString paramsheader;
	QString paramstype;
	QString outputdataheader;
	QString outputdatatype;
	QString varsheader;
	QString varstype;
	QStringList inputheaders;
	QStringList inputparamstypes;
	QStringList inputdatatypes;
	int outputportsnumber;
	QList<int> inputportssize;
	QStringList inputportsparamstypes;
	QStringList inputportsdatatypes;
	QStringList exfuncs;
	QString outputvcxproj;
	QString outputdir;
protected:
	bool readConfiguration();
	void createFiles();
	void writeHead(QTextStream & textstream, QString suffix);
	void writeTail(QTextStream & textstream);
	void writeClass(QTextStream & textstream, QString suffix, QString inherit=QString());
	QString getText(QDomElement & tmproot, QStringList tags);
	QStringList getTexts(QDomElement & tmproot, QStringList tags);
	void writePubFuncHeader(QTextStream & textstream);
	void writePubFuncCpp(QTextStream & textstream);
	void writePrivFuncHeader(QTextStream & textstream);
	void writePrivFuncCpp(QTextStream & textstream);
	void writePrivCoreFuncHeader(QTextStream & textstream);
	void writePrivCoreFuncCpp(QTextStream & textstream);
	void writePrivExFuncHeader(QTextStream & textstream);
	void writePrivExFuncCpp(QTextStream & textstream);
	void replaceText(QString & text);
	void configProject();
	void setText(QDomDocument * tmpdoc, QDomElement & tmproot, QString tag, QString text);
};

#endif // SCANINTERFACEFUNCTION_H
