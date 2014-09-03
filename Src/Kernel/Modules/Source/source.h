#ifndef SOURCE_H
#define SOURCE_H

/*! \defgroup Source_Library Source_Library
	\ingroup Node_Library
	\brief The Library of Source.
*/

/*! \defgroup Source
	\ingroup Modules
	\brief The Source node.
*/

/*! \defgroup ExSource
	\ingroup ExModules
	\brief The ExSource node.
*/

/*! \addtogroup Source 
	@{
*/

/*! \file source.h
	\brief Defines the Source node.
	\details A node can only generate source data.
*/

#include<Core/Node/node.h>

/*!	\def SOURCESLOT
	\brief MACRO of Source::generateSourceDataSlot() or SourceDrain::generateSourceDataSlot() to receive trigger signal.
*/
#ifndef SOURCESLOT
#define SOURCESLOT SLOT(generateSourceDataSlot())
#endif

/*! \class Source
	\brief Source is one of the four basic modules. It can only generate source data.
	\details
	Source: \image html Source.png
	There are two kinds of extended modules:
	- Sensor: source data collected by sensors.
	- Simulator: source data generated by data file according to the timestamp.
	
	Remarks:
	- Source can be used directly in the application.
	- Provides 2 interface function:
		- [private] Source::initializeOutputData
		- [private] Source::generateSourceData
	- Provides 1 set of Qt Signal-Slot:
		-Generate source data
			- [slot] virtual Source::generateSourceDataSlot();
			- [signal] Source::generateSourceDataSignal();
			- [signal] Source::generateSourceDataErrorSignal();
*/
class Source : public Node
{
	Q_OBJECT
public:
	/*! \fn Source(QString qstrSharedLibrary, QString qstrNodeType, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx)
		\brief The constructor of the class Source. (For extended modules)
		\param [in] qstrSharedLibrary The name of the shared library.
		\param [in] qstrNodeType The type-name of the node.
		\param [in] qstrNodeClass The class-name of the node.
		\param [in] qstrNodeName The node-name of the node.
		\param [in] qstrConfigName The name of the config file.
		\param [in] qstrFuncEx The extension of Source::generateSourceData.
		\details
		- Load and check the shared library.
		- Check input and output ports.
	*/
	Source(QString qstrSharedLibrary, QString qstrNodeType, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx);
	/*! \fn Source(QString qstrSharedLibrary, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx=QString())
		\brief The constructor of the class Source. (For directly using)
		\param [in] qstrSharedLibrary The name of the shared library.
		\param [in] qstrNodeClass The class-name of the node.
		\param [in] qstrNodeName The node-name of the node.
		\param [in] qstrConfigName The name of the config file.
		\param [in] qstrFuncEx The extension of Source::generateSourceData.
		\details
		- Set the type-name as "Source"
		- Load and check the shared library.
		- Check input and output ports.
	*/
	Source(QString qstrSharedLibrary, QString qstrNodeClass, QString qstrNodeName, QString qstrConfigName, QString qstrFuncEx=QString());
protected:
	/*! \typedef void (*initializeOutputDataFptr)(void * paramsPtr, void * varsPtr, boost::shared_ptr<void> & outputDataPtr)
		\brief [required] Function pointer type for interface function of initializing node's output data.
		\param [in] paramsPtr The node's parameters(\ref Node::paramsptr).
		\param [in] varsPtr The node's variables(\ref Node::varsptr).
		\param [out] outputDataPtr The output data embelished by boost::shared_pointer<void>.
		\details To initialize output data:
		- outputDataPtr=boost::shared_ptr<void> (new OutputDataType);
	*/
	typedef void (*initializeOutputDataFptr)(void * paramsPtr, void * varsPtr, boost::shared_ptr<void> & outputDataPtr);
	/*! \typedef bool (*generateSourceDataFptr)(void * paramsPtr, void * varsPtr, void * outputData, QList<int> & outputPortIndex, QTime & timeStamp)
		\brief [required] Function pointer type for interface function of generating source data.
		\param [in] paramsPtr The node's parameters(\ref Node::paramsptr).
		\param [in] varsPtr The node's variables(\ref Node::varsptr).
		\param [out] outputData The output data.
		\param [out] outputPortIndex The index of output port to send output data.
		\param [out] timeStamp The timestamp. Mainly used in simulator to get the timestamp of one data frame.
		\return 1 for success and 0 for failure.
	*/
	typedef bool (*generateSourceDataFptr)(void * paramsPtr, void * varsPtr, void * outputData, QList<int> & outputPortIndex, QTime & timeStamp);
protected:
	/*! \var initializeOutputData
		\brief [private] Interface function of initializing node's output data.
	*/
	initializeOutputDataFptr initializeOutputData;
	/*! \var generateSourceData
		\brief [private] Interface function of generating source data.
	*/
	generateSourceDataFptr generateSourceData;
public slots:
	/*! \fn virtual void generateSourceDataSlot()
		\brief The slot function for generating source data.
		\details
		Overloaded by Simulator to realize self-trigger.
	*/
	virtual void generateSourceDataSlot();
signals:
	/*! \fn void generateSourceDataSignal();
		\brief The signal function for generating source data normally.
	*/
	void generateSourceDataSignal();
	/*! \fn void generateSourceDataErrorSignal();
		\brief The signal function for generating source data with error.
	*/
	void generateSourceDataErrorSignal();
};

/*! @}*/

#endif // SOURCE_H
