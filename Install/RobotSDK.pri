include($${PROJNAME}.pri)

INCLUDEPATH += .

unix{    
    equals(INSTALLTYPE, "SDK") {
	CONFIG(debug, debug|release){
	    TARGET = $$PROJNAME
	    DESTDIR = ../Build/lib/Debug
	    target.path = $$(HOME)/$$INSTALLTYPE/$$TARGET/lib/Debug
	}
	else {
	    TARGET = $$PROJNAME
	    DESTDIR = ../Build/lib/Release
            target.path = $$(HOME)/$$INSTALLTYPE/$$TARGET/lib/Release
	}
        INSTALL_PREFIX = $$(HOME)/$$INSTALLTYPE/$$TARGET/include
        INSTALL_HEADERS = $$HEADERS
        include(Install.pri)
    }
    equals(INSTALLTYPE, "APP") {
	INCLUDEPATH += $$(HOME)/SDK/RobotSDK/Kernel/include
	CONFIG(debug, debug|release){
	    TARGET = $$PROJNAME
	    DESTDIR = ../Build/Debug
	    target.path = $$(HOME)/$$INSTALLTYPE/$$TARGET/Debug
	    LIBS += -L/home/alexanderhmw/SDK/RobotSDK/Kernel/lib/Debug -lKernel
	}
	else {
	    TARGET = $$PROJNAME
	    DESTDIR = ../Build/Release
            target.path = $$(HOME)/$$INSTALLTYPE/$$TARGET/Release
	    LIBS += -L/home/alexanderhmw/SDK/RobotSDK/Kernel/lib/Release -lKernel
	}
    }
    equals(INSTALLTYPE, "MOD") {
	INCLUDEPATH += $$(HOME)/SDK/RobotSDK/Kernel/include
        INCLUDEPATH += ..
	DESTDIR = ../Build/
	target.path = $$(HOME)/SDK/RobotSDK/Module/SharedLibrary
        CONFIG(debug, debug|release){
	    TARGET = $$PROJNAME_Debug
	    LIBS += -L/home/alexanderhmw/SDK/RobotSDK/Kernel/lib/Debug -lKernel	
	}
	else {
	    TARGET = $$PROJNAME
	    LIBS += -L/home/alexanderhmw/SDK/RobotSDK/Kernel/lib/Release -lKernel
	}
	DEFINES += RobotSDK_ModuleDev
    }

    INSTALLS += target
}

win32{
    equals(INSTALLTYPE, "SDK") {
	CONFIG(debug, debug|release){
	    TARGET = $$PROJNAME
	    DESTDIR = ../Build/lib/Debug
	}
	else {
	    TARGET = $$PROJNAME
	    DESTDIR = ../Build/lib/Release
	}
    }
    equals(INSTALLTYPE, "APP") {
	TMPPATH=$$(RobotDep_Include)
        isEmpty(TMPPATH) {
            error($$TMPPATH is not Specified.)
        }
        else{
            INCLUDEPATH += $$split(TMPPATH,;)
        }
        TMPPATH=$$(RobotSDK_Kernel)
        isEmpty(TMPPATH) {
            error($$TMPPATH is not Specified.)
        }
        else{
            INCLUDEPATH += $$TMPPATH/include
        }
	CONFIG(debug, debug|release){
	    TARGET = $$PROJNAME
	    DESTDIR = ../Build/Debug
	    LIBS += $$(RobotSDK_Kernel)/lib/Debug/Kernel.lib
	}
	else {
	    TARGET = $$PROJNAME
	    DESTDIR = ../Build/Release
	    LIBS += $$(RobotSDK_Kernel)/lib/Release/Kernel.lib
	}
    }
    equals(INSTALLTYPE, "MOD") {
	TMPPATH=$$(RobotDep_Include)
        isEmpty(TMPPATH) {
            error($$TMPPATH is not Specified.)
        }
        else{
            INCLUDEPATH += $$split(TMPPATH,;)
        }
        TMPPATH=$$(RobotSDK_Kernel)
        isEmpty(TMPPATH) {
            error($$TMPPATH is not Specified.)
        }
        else{
            INCLUDEPATH += $$TMPPATH/include
        }
	DESTDIR = ../Build/
        CONFIG(debug, debug|release){
	    TARGET = $$PROJNAME_Debug
	    LIBS += $$(RobotSDK_Kernel)/lib/Debug/Kernel.lib
	}
	else {
	    TARGET = $$PROJNAME
	    LIBS += $$(RobotSDK_Kernel)/lib/Release/Kernel.lib
	}
	DEFINES += RobotSDK_ModuleDev
    }    
}
