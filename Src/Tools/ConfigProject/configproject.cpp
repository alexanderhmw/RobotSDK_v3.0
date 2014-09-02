#include "configproject.h"

ConfigProject::ConfigProject(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.Browse,SIGNAL(clicked()),this,SLOT(browseSlot()));
	connect(ui.Config,SIGNAL(clicked()),this,SLOT(configSlot()));

	vversion<<"v120"<<"v110"<<"v100"<<"v90"<<"v80";
	formatversion<<"12.0"<<"11.00"<<"10.00"<<"9.0"<<"8.0";
	vsversion<<"2013"<<"2012"<<"2010"<<"2009"<<"2008";
	qtex32<<"msvc2012"<<"msvc2010_opengl";
	qtex64<<"msvc2012_64_opengl"<<"$(DefaultQtVersion)";
	qt5version<<"Qt5Version_x0020_Win32"<<"Qt5Version_x0020_x64";
	ui.VSVersion->addItems(vversion);
}

ConfigProject::~ConfigProject()
{

}


void ConfigProject::browseSlot()
{
	QString startdir;
	if(ui.Application->isChecked())
	{
		startdir=QString();
	}
	else if(ui.Library->isChecked())
	{
		startdir=QString(getenv("RobotSDK_ModuleDev"));
	}
	else if(ui.Kernel->isChecked())
	{
		startdir=QString(getenv("RobotSDK_Kernel"));
	}
	else
	{
		return;
	}
	QString projectsdir=QFileDialog::getExistingDirectory(this,"Project Dir",startdir);
	ui.ProjectsDir->setText(projectsdir);
}

void ConfigProject::configSlot()
{
	QString projectsdir=ui.ProjectsDir->text();
	configProjects(projectsdir);
	QMessageBox::information(this,"Finish","Finish Configuration.");
}

void ConfigProject::setText(QDomDocument * tmpdoc, QDomElement & tmproot, QString tag, QString text)
{
	QDomElement curelem=tmproot.firstChildElement(tag);
	if(curelem.isNull())
	{
		curelem=tmproot.appendChild(tmpdoc->createElement(tag)).toElement();
	}
	if(curelem.hasChildNodes())
	{
		QDomNode tmpnode=curelem.firstChild();
		while(!tmpnode.isNull()&&!tmpnode.isText())
		{
			tmpnode=tmpnode.nextSibling();
		}
		if(!tmpnode.isNull())
		{
			tmpnode.toText().setData(text);
		}
		else
		{
			curelem.appendChild(tmpdoc->createTextNode(text));
		}
	}
	else
	{
		curelem.appendChild(tmpdoc->createTextNode(text));
	}
}

void ConfigProject::configProjects(QString projectsdir)
{
	QDir dir(projectsdir);
	if(!dir.exists())
	{
		return;
	}
	dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
	dir.setSorting(QDir::DirsFirst);
	QFileInfoList list=dir.entryInfoList();
	int i,n=list.size();
	for(i=0;i<n;i++)
	{
		QFileInfo fileinfo=list.at(i);
		QString filename=fileinfo.absoluteFilePath();
		if(fileinfo.isDir()&&ui.Recursive->isChecked())
		{
			configProjects(filename);
		}
		else if(fileinfo.fileName().endsWith(".vcxproj"))
		{
			configProject(filename);
		}
		else if(fileinfo.fileName().endsWith(".sln"))
		{
			configSolution(filename);
		}
	#ifdef Q_OS_LINUX
        else if(fileinfo.fileName().endsWith(".pro"))
        {
            configQtPro(filename);
        }
	#elif defined(Q_OS_WIN)
	#endif
	}
}

void ConfigProject::configProject(QString projectname)
{
	QDomDocument * projectdoc=new QDomDocument();
	QFile file(projectname);
	if(file.open(QIODevice::ReadOnly|QIODevice::Text))
	{
		if(!projectdoc->setContent(&file))
		{
			delete projectdoc;
			file.close();
			return;
		}
		file.close();
	}
	else
	{
		return;
	}
	QStringList conditions;
#ifdef Q_PROCESSOR_X86_64
    conditions<<"\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'";
    conditions<<"\'$(Configuration)|$(Platform)\'==\'Release|Win32\'";
    conditions<<"\'$(Configuration)|$(Platform)\'==\'Debug|x64\'";
    conditions<<"\'$(Configuration)|$(Platform)\'==\'Release|x64\'";
#elif Q_PROCESSOR_X86
    conditions<<"\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'";
    conditions<<"\'$(Configuration)|$(Platform)\'==\'Release|Win32\'";
#endif
	int j,m=conditions.size();
	QDomElement projectroot=projectdoc->documentElement();
	QDomElement propertygroup=projectroot.firstChildElement("PropertyGroup");

	while(propertygroup.attribute("Label")!="Globals")
	{
		propertygroup=propertygroup.nextSiblingElement("PropertyGroup");
	}
	if(propertygroup.isNull())
	{
		propertygroup=projectroot.appendChild(projectdoc->createElement("PropertyGroup")).toElement();
		propertygroup.setAttribute("Label","Globals");
	}
	propertygroup=propertygroup.nextSiblingElement("PropertyGroup");

	for(j=0;j<m;j++)
	{
		if(propertygroup.isNull())
		{
			propertygroup=projectroot.appendChild(projectdoc->createElement("PropertyGroup")).toElement();
		}
		else if(propertygroup.attribute("Label")=="UserMacros")
		{
			propertygroup=projectroot.insertBefore(projectdoc->createElement("PropertyGroup"),propertygroup).toElement();
		}
		propertygroup.setAttribute("Condition",conditions.at(j));
		setText(projectdoc,propertygroup,"PlatformToolset",ui.VSVersion->currentText());
		propertygroup=propertygroup.nextSiblingElement("PropertyGroup");
	}

	while(propertygroup.attribute("Label")!="UserMacros")
	{
		propertygroup=propertygroup.nextSiblingElement("PropertyGroup");
	}
	if(propertygroup.isNull())
	{
		propertygroup=projectroot.appendChild(projectdoc->createElement("PropertyGroup")).toElement();
		propertygroup.setAttribute("Label","UserMacros");
	}
	propertygroup=propertygroup.nextSiblingElement("PropertyGroup");

	for(j=0;j<m;j++)
	{
		if(propertygroup.isNull())
		{
			propertygroup=projectroot.appendChild(projectdoc->createElement("PropertyGroup")).toElement();
		}
		propertygroup.setAttribute("Condition",conditions.at(j));
		if(ui.Library->isChecked())
		{
            setText(projectdoc,propertygroup,"OutDir","$(RobotSDK_SharedLibrary)");
		}
		else if(ui.Kernel->isChecked())
		{
            setText(projectdoc,propertygroup,"OutDir","$(RobotSDK_Kernel)\\lib");
		}
		else if(ui.Application->isChecked())
		{
			setText(projectdoc,propertygroup,"OutDir","$(SolutionDir)\\$(PlatformToolset)\\$(Platform)\\$(Configuration)\\");
		}
        setText(projectdoc,propertygroup,"IncludePath","$(RobotSDK_Kernel)\\include;$(RobotSDK_ModuleDev);$(RobotSDK_Module);$(RobotDep_Include);$(IncludePath)");
        setText(projectdoc,propertygroup,"LibraryPath","$(RobotSDK_Kernel)\\lib\\$(Configuration);$(RobotDep_Lib);$(LibraryPath)");
		propertygroup=propertygroup.nextSiblingElement("PropertyGroup");
	}
	while(!propertygroup.isNull())
	{
		QDomElement tmpelem=propertygroup.nextSiblingElement("PropertyGroup");
		projectroot.removeChild(propertygroup).clear();
		propertygroup=tmpelem;
	}

	QDomElement itemdefine=projectroot.firstChildElement("ItemDefinitionGroup");
	while(!itemdefine.isNull())
	{
		int conditionsid=conditions.indexOf(itemdefine.attribute("Condition"));
		if(conditionsid<0)
		{
			continue;
		}
		QDomElement predefineelem=itemdefine.firstChildElement("ClCompile").firstChildElement("PreprocessorDefinitions");
		QString predefine=predefineelem.text();

		if(ui.Library->isChecked())
		{
            if(!predefine.contains("RobotSDK_ModuleDev"))
			{
                predefine=QString("RobotSDK_ModuleDev;")+predefine;
			}
		}
		else
		{
            if(predefine.contains("RobotSDK_ModuleDev"))
			{
                predefine.remove(QString("RobotSDK_ModuleDev;"));
                predefine.remove(QString("RobotSDK_ModuleDev"));
			}
		}
		{
			QDomNode tmpnode=predefineelem.firstChild();
			while(!tmpnode.isNull()&&!tmpnode.isText())
			{
				tmpnode=tmpnode.nextSibling();
			}
			if(!tmpnode.isNull())
			{
				tmpnode.toText().setData(predefine);
			}
		}
		if(ui.Library->isChecked())
		{
			QDomElement outputfileelem=itemdefine.firstChildElement("Link").firstChildElement("OutputFile");
			QString outputfile=outputfileelem.text();
			outputfile=QString("$(OutDir)\\$(ProjectName)_$(PlatformToolset)_$(Platform)_$(Configuration).dll");
			QDomNode tmpnode=outputfileelem.firstChild();
			while(!tmpnode.isNull()&&!tmpnode.isText())
			{
				tmpnode=tmpnode.nextSibling();
			}
			if(!tmpnode.isNull())
			{
				tmpnode.toText().setData(outputfile);
			}
		}
		itemdefine=itemdefine.nextSiblingElement("ItemDefinitionGroup");
	}

	//QDomElement userproperties=projectroot.firstChildElement("ProjectExtensions").firstChildElement("VisualStudio").firstChildElement("UserProperties");
	//if(!userproperties.isNull())
	//{
	//	userproperties.setAttribute(qt5version.at(0),qtex32.at(ui.VSVersion->currentIndex()));
	//	userproperties.setAttribute(qt5version.at(1),qtex64.at(ui.VSVersion->currentIndex()));
	//}

	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream textstream(&file);
	projectdoc->save(textstream,2);
	file.close();
	delete projectdoc;
}

void ConfigProject::configSolution(QString solutionname)
{
	QFile file(solutionname);
	QStringList solution;
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		QString tmpstr1=QString("Microsoft Visual Studio Solution File, Format Version ");
		QString tmpstr2=QString("# Visual Studio ");
		while(!stream.atEnd())
		{
			QString content=stream.readLine();
			if(content.startsWith(tmpstr1))
			{
				content=tmpstr1+formatversion.at(ui.VSVersion->currentIndex());
			}
			else if(content.startsWith(tmpstr2))
			{
				content=tmpstr2+vsversion.at(ui.VSVersion->currentIndex());
			}
			solution<<content;
		}
	}
	file.close();
	if(file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		int i,n=solution.size();
		for(i=0;i<n;i++)
		{
			stream<<solution.at(i)<<"\n";
		}
	}
	file.close();
}

void ConfigProject::configQtPro(QString proname)
{
    QString filename=proname;
    QFile file(filename);
    QString sources;
    QString headers;
    QString otherfiles;
    QString includepath;
    QString unixlibs;
    QString win32libs;
    QString forms;
    QString resources;
    QString installheaders;
    QString defines;
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        while(!file.atEnd())
        {
            QString tmpline=file.readLine();
            while(!file.atEnd()&&tmpline.endsWith("\\\n"))
            {
                tmpline+=file.readLine();
            }
            if(tmpline.endsWith("\n"))
            {
                tmpline.truncate(tmpline.size()-1);
            }
            if(tmpline.startsWith("SOURCES +="))
            {
                sources=tmpline;
            }
            else if(tmpline.startsWith("HEADERS +="))
            {
                headers=tmpline;
            }
            else if(tmpline.startsWith("OTHER_FILES +="))
            {
                otherfiles=tmpline;
            }
            else if(tmpline.startsWith("INCLUDEPATH +="))
            {
                includepath=tmpline;
            }
            else if(tmpline.startsWith("unix:LIBS +="))
            {
                unixlibs=tmpline;
            }
            else if(tmpline.startsWith("win32:LIBS +="))
            {
                win32libs=tmpline;
            }
            else if(tmpline.startsWith("FORMS +="))
            {
                forms=tmpline;
            }
            else if(tmpline.startsWith("RESOURCES +="))
            {
                resources=tmpline;
            }
            else if(tmpline.startsWith("INSTALLHEADERS +="))
            {
                installheaders=tmpline;
            }
            else if(tmpline.startsWith("DEFINES +="))
            {
                defines=tmpline;
            }
        }
        file.close();
    }
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        return;
    }
    QTextStream stream(&file);
    stream<<"#-------------------------------------------------\n";
    stream<<"#\n";
    stream<<QString("# Project created by QtCreator %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss"));
    stream<<"#\n";
    stream<<"#-------------------------------------------------\n\n";

    stream<<"QT += widgets network opengl xml\n\n";
    if(ui.Library->isChecked())
    {
        stream<<"CONFIG += shared qt\n";
        stream<<"TEMPLATE = lib\n";
    }
    else if(ui.Kernel->isChecked())
    {
        stream<<"CONFIG += staticlib qt\n";
        stream<<"TEMPLATE = lib\n";
    }
    else if(ui.Application->isChecked())
    {
        stream<<"CONFIG += qt\n";
        stream<<"TEMPLATE = app\n";
    }

    QString target=filename.mid(filename.lastIndexOf("/")+1);
    target.truncate(target.lastIndexOf(".pro"));
    QString relativepath=filename;
    relativepath.truncate(filename.lastIndexOf("/"));
    relativepath.remove(QString("%1/").arg(ui.ProjectsDir->text()));
    stream<<"CONFIG(debug, debug|release){\n";
    stream<<QString("\tTARGET = %1_Debug\n").arg(target);
    stream<<QString("\tMOC_DIR = $$(HOME)/SDK/RobotSDK/ModuleDev/Build/tmp/%1/debug\n").arg(relativepath);
    stream<<QString("\tOBJECTS_DIR = $$(HOME)/SDK/RobotSDK/ModuleDev/Build/tmp/%1/debug\n").arg(relativepath);
    stream<<"\tunix:LIBS *= /opt/RobotSDK/Kernel/lib/libKernel_Debug.a\n";
    stream<<"\twin32:LIBS *= $$(RobotSDK_Kernel)/lib/libKernel_Debug.lib\n";
    stream<<"}\n";
    stream<<"else{\n";
    stream<<QString("\tTARGET = %1\n").arg(target);
    stream<<QString("\tMOC_DIR = $$(HOME)/SDK/RobotSDK/ModuleDev/Build/tmp/%1/release\n").arg(relativepath);
    stream<<QString("\tOBJECTS_DIR = $$(HOME)/SDK/RobotSDK/ModuleDev/Build/tmp/%1/release\n").arg(relativepath);
    stream<<"\tunix:LIBS *= /opt/RobotSDK/Kernel/lib/libKernel.a\n";
    stream<<"\twin32:LIBS *= $$(RobotSDK_Kernel)/lib/libKernel.lib\n";
    stream<<"}\n\n";

    stream<<"DEFINES *= RobotSDK_ModuleDev\n";
    stream<<"DESTDIR = $$(HOME)/SDK/RobotSDK/ModuleDev/Build/SharedLibrary\n\n";
    if(sources.size()>0)
    {
        stream<<sources<<"\n\n";
    }
    if(headers.size()>0)
    {
        stream<<headers<<"\n\n";
    }
    if(otherfiles.size()>0)
    {
        stream<<otherfiles<<"\n\n";
    }
    if(includepath.size()>0)
    {
        stream<<includepath<<"\n\n";
    }
    if(unixlibs.size()>0)
    {
        stream<<unixlibs<<"\n\n";
    }
    if(win32libs.size()>0)
    {
        stream<<win32libs<<"\n\n";
    }
    if(forms.size()>0)
    {
        stream<<forms<<"\n\n";
    }
    if(resources.size()>0)
    {
        stream<<resources<<"\n\n";
    }
    if(installheaders.size()>0)
    {
        stream<<installheaders<<"\n\n";
    }
    if(defines.size()>0)
    {
        stream<<defines<<"\n\n";
    }
    stream<<"unix{\n";
    stream<<"\tINCLUDEPATH *= \t\\\n";
    stream<<"\t\t.\t\\\n";
    stream<<"\t\t/usr/include\t\\\n";
    stream<<"\t\t/opt/RobotSDK/Kernel/include\t\\\n";
    stream<<"\t\t$$(HOME)/SDK/RobotSDK/ModuleDev\t\\\n";
    stream<<"\t\t/opt/RobotSDK/Module/include\t\\\n\n";
    stream<<"}\n\n";
    stream<<"win32{\n";
    stream<<"\tINCLUDEPATH *= \t\\\n";
    stream<<"\t\t.\t\\\n";
    stream<<"\t\t/opt/RobotSDK/Kernel/include\t\\\n";
    stream<<"\t\t$$(HOME)/SDK/RobotSDK/ModuleDev\t\\\n";
    stream<<"\t\t/opt/RobotSDK/Module/include\t\\\n\n";
    stream<<"}\n\n";
    stream<<"target.path = /opt/RobotSDK/Module/SharedLibrary\n";
    stream<<"INSTALLS += target\n\n";
    stream<<"INSTALL_PREFIX = /opt/RobotSDK/Module/include\n";
    stream<<"INSTALL_HEADERS = $$INSTALLHEADERS\n";
    stream<<"include(Kernel.pri)\n\n";
    file.close();
    QString outputdir=proname.left(proname.lastIndexOf("/"));
    file.setFileName(QString("%1/Kernel.pri").arg(outputdir));
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        return;
    }
    stream.setDevice(&file);
    stream<<"for(header, INSTALL_HEADERS) {\n";
    stream<<"\tpath = $${INSTALL_PREFIX}/$${dirname(header)}\n";
    stream<<"\teval(headers_$${path}.files += $$header)\n";
    stream<<"\teval(headers_$${path}.path = $$path)\n";
    stream<<"\teval(INSTALLS *= headers_$${path})\n";
    stream<<"}\n";
    file.close();
    return;
}
