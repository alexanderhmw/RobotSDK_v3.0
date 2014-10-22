INCLUDEPATH += .

unix{    
    equals(INSTALLTYPE, "SDK") {
        include($${PROJNAME}.pri)
        CONFIG(debug, debug|release){
			TARGET = $${PROJNAME}_Debug
			DESTDIR = ../Build/lib
			target.path = $$(HOME)/$$INSTALLTYPE/$$TARGET/lib
		}
		else {
			TARGET = $${PROJNAME}_Release
			DESTDIR = ../Build/lib
			target.path = $$(HOME)/$$INSTALLTYPE/$$TARGET/lib
		}
        INSTALL_PREFIX = $$(HOME)/$$INSTALLTYPE/$$TARGET/include
        INSTALL_HEADERS = $$HEADERS
        include(RobotSDK_Install.pri)
    }
    equals(INSTALLTYPE, "APP") {
		INCLUDEPATH += $$(HOME)/SDK/RobotSDK/Kernel/include
		CONFIG(debug, debug|release){
			TARGET = $$PROJNAME
			DESTDIR = ../Build/Debug
			target.path = $$(HOME)/$$INSTALLTYPE/$$TARGET/Debug
			LIBS += -L$$(HOME)/SDK/RobotSDK/Kernel/lib/Debug -lKernel
		}
		else {
			TARGET = $$PROJNAME
			DESTDIR = ../Build/Release
            target.path = $$(HOME)/$$INSTALLTYPE/$$TARGET/Release
			LIBS += -L$$(HOME)/SDK/RobotSDK/Kernel/lib/Release -lKernel
		}
    }
    equals(INSTALLTYPE, "MOD") {
        include($${PROJNAME}.pri)
		INCLUDEPATH += $$(HOME)/SDK/RobotSDK/Kernel/include
		INCLUDEPATH += $$(HOME)/SDK/RobotSDK/ModuleDev
		target.path = $$(HOME)/SDK/RobotSDK/Module/SharedLibrary
        CONFIG(debug, debug|release){
			DESTDIR = ../Build/
			TARGET = $${PROJNAME}_Debug
			LIBS += -L$$(HOME)/SDK/RobotSDK/Kernel/lib/Debug -lKernel	
		}
		else {
			DESTDIR = ../Build/
            TARGET = $${PROJNAME}_Release
			LIBS += -L$$(HOME)/SDK/RobotSDK/Kernel/lib/Release -lKernel
		}
		DEFINES += RobotSDK_ModuleDev
    }

    INSTALLS += target
}

win32{
    equals(INSTALLTYPE, "SDK") {
		include($${PROJNAME}.pri)
		CONFIG(debug, debug|release){
			TARGET = $${PROJNAME}_Debug
			DESTDIR = ../Build/lib
		}
		else {
			TARGET = $${PROJNAME}_Release
			DESTDIR = ../Build/lib
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
		DEFINES -= RobotSDK_ModuleDev
    }
    equals(INSTALLTYPE, "MOD") {
		include($${PROJNAME}.pri)
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
		TMPPATH=$$(RobotSDK_ModuleDev)
        isEmpty(TMPPATH) {
            error($$TMPPATH is not Specified.)
        }
        else{
            INCLUDEPATH += $$TMPPATH
        }
		DESTDIR = ../Build/
        CONFIG(debug, debug|release){
			TARGET = $${PROJNAME}_Debug
			LIBS += $$(RobotSDK_Kernel)/lib/Debug/Kernel.lib
		}
		else {
            TARGET = $${PROJNAME}_Release
			LIBS += $$(RobotSDK_Kernel)/lib/Release/Kernel.lib
		}
		DEFINES += RobotSDK_ModuleDev
    }    
}
