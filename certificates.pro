QT += widgets network printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    certeditor/certdocument.cpp \
    certeditor/certeditor.cpp \
    certeditor/certprocessor.cpp \
    dialogauthor/dialogauthor.cpp \
    dialogcopyvalel/dialogcopyvalel.cpp \
    dialoglabels/dialoglabels.cpp \
    formdoc/formdoc.cpp \
    formdoc/modeldoc.cpp \
    formmarkel/formmarkel.cpp \
    formpartel/formpartel.cpp \
    formpartwire/formpartwire.cpp \
    formpartwire/modelwirepartimech.cpp \
    formship/formship.cpp \
    main.cpp \
    mainwindow.cpp \
    pasportmanager.cpp \
    progressreportdialog/progressreportdialog.cpp \
    rest/relmodels.cpp \
    rest/restcombobox.cpp \
    rest/restconnection.cpp \
    rest/restdateedit.cpp \
    rest/resterror.cpp \
    rest/restfilter.cpp \
    rest/restitemdelegate.cpp \
    rest/restlogin.cpp \
    rest/restmapper.cpp \
    rest/restrelmodel.cpp \
    rest/restrotablemodel.cpp \
    rest/resttabledialog.cpp \
    rest/resttablemodel.cpp \
    rest/resttableview.cpp \
    rest/tempfilemanager.cpp \
    tabmanager.cpp

HEADERS += \
    certeditor/certdocument.h \
    certeditor/certeditor.h \
    certeditor/certprocessor.h \
    dialogauthor/dialogauthor.h \
    dialogcopyvalel/dialogcopyvalel.h \
    dialoglabels/dialoglabels.h \
    formdoc/formdoc.h \
    formdoc/modeldoc.h \
    formmarkel/formmarkel.h \
    formpartel/formpartel.h \
    formpartwire/formpartwire.h \
    formpartwire/modelwirepartimech.h \
    formship/formship.h \
    mainwindow.h \
    pasportmanager.h \
    progressreportdialog/progressreportdialog.h \
    rest/relmodels.h \
    rest/restcombobox.h \
    rest/restconnection.h \
    rest/restdateedit.h \
    rest/resterror.h \
    rest/restfilter.h \
    rest/restitemdelegate.h \
    rest/restlogin.h \
    rest/restmapper.h \
    rest/restrelmodel.h \
    rest/restrotablemodel.h \
    rest/resttabledialog.h \
    rest/resttablemodel.h \
    rest/resttableview.h \
    rest/resttypes.h \
    rest/tempfilemanager.h \
    tabmanager.h

FORMS += \
    certeditor/certeditor.ui \
    dialogauthor/dialogauthor.ui \
    dialogcopyvalel/dialogcopyvalel.ui \
    dialoglabels/dialoglabels.ui \
    formdoc/formdoc.ui \
    formmarkel/formmarkel.ui \
    formpartel/formpartel.ui \
    formpartwire/formpartwire.ui \
    formship/formship.ui \
    mainwindow.ui \
    progressreportdialog/progressreportdialog.ui \
    rest/restlogin.ui \
    rest/resttabledialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:RC_FILE = ico.rc
macx:ICON = ico.icns

RESOURCES += \
    formdoc/res.qrc \
    res.qrc
