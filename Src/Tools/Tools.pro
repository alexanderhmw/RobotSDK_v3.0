TEMPLATE = subdirs

unix{
SUBDIRS += \
    ConfigSystem \
    ConfigModule \
#   ConfigProject \
#   ConfigApplication
}

win32{
SUBDIRS += \
    ConfigSystem \
    ConfigModule \
    ConfigProject \
#   ConfigApplication
}