QT       += core gui
QT       += webenginecore webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(lib/qcustomplot.pri)

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/dlgconfig.cpp \
    src/dlghistory.cpp \
    src/historymodel.cpp

HEADERS += \
    src/mainwindow.h \
    src/dlgconfig.h \
    src/dlghistory.h \
    src/historymodel.h

FORMS += \
    src/mainwindow.ui \
    src/dlgconfig.ui \
    src/dlghistory.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    WebLoad.qrc

unix:!android:{
    DESKTOP_FILES.files = \
        WebLoadTest.desktop
    DESKTOP_FILES.path += /usr/share/applications/
    IMAGES_FILES.files = \
        images/webload.png
    IMAGES_FILES.path += /usr/share/pixmaps/
    INSTALLS += DESKTOP_FILES IMAGES_FILES
}

win32 {
    CONFIG += windeployqt
# windows resources
    RC_ICONS=$$PWD/images/webload.ico #：指定應該被包含進一個.rc檔案中的圖示，僅適用於Windows
    #QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'

    QMAKE_TARGET_PRODUCT=$${TARGET} #：指定項目目標的產品名稱，僅適用於Windows
    QMAKE_TARGET_DESCRIPTION="Web load test tool" #：指定項目目標的描述資訊，僅適用於Windows
    #PACKAGE_DOMAIN：
    #PACKAGE_VERSION：
    RC_CODEPAGE=0x04b0 #unicode：指定應該被包含進一個.rc檔案中的字碼頁，僅適用於Windows
    RC_LANG=0x0409 #en_US：指定應該被包含進一個.rc檔案中的語言，僅適用於Windows

    CONFIG(debug, debug|release) {
        DESTDIR = Debug
    } else {
        DESTDIR = Release
    }

    DISTFILES += $$PWD/images/webload.ico

    DIST_DIRECTORY =  $$shell_quote($$shell_path($${PWD}/../$${TARGET}_$${QT_ARCH}))

    DIST_FILE = $$shell_quote($$shell_path($$DIST_DIRECTORY/$${TARGET}.exe))

}
