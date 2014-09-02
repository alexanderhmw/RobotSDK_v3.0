#ifndef CONFIGPROJECT_H
#define CONFIGPROJECT_H

#include <QtWidgets/QMainWindow>
#include "ui_configproject.h"

#include<qdom.h>
#include<qstring.h>
#include<qlist.h>
#include<qfiledialog.h>
#include<qfile.h>
#include<qtextstream.h>
#include<qmessagebox.h>
#include<qdatetime.h>

class ConfigProject : public QMainWindow
{
	Q_OBJECT

public:
	ConfigProject(QWidget *parent = 0);
	~ConfigProject();
private:
	Ui::ConfigProjectClass ui;
protected:
	QStringList vversion;
	QStringList formatversion;
	QStringList vsversion;
	QStringList qtex32;
	QStringList qtex64;
	QStringList qt5version;
public slots:
	void browseSlot();
	void configSlot();
protected:
	void setText(QDomDocument * tmpdoc, QDomElement & tmproot, QString tag, QString text);
	void configProjects(QString projectsdir);
	void configProject(QString projectname);
	void configSolution(QString solutionname);
    void configQtPro(QString proname);
};

#endif // CONFIGPROJECT_H
