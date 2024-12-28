QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    a_all_template_window.cpp \
    a_list_template_window.cpp \
    a_tasks_template_window.cpp \
    b_main_window.cpp \
    c_promanawindow.cpp \
    c_promanawindow_newpro.cpp \
    c_promanawindow_newpro_dialog.cpp \
    main.cpp \
    a_login_windows.cpp

HEADERS += \
    a_all_template_window.h \
    a_list_template_window.h \
    a_login_windows.h \
    a_tasks_template_window.h \
    b_main_window.h \
    c_promanawindow.h \
    c_promanawindow_newpro.h \
    c_promanawindow_newpro_dialog.h

FORMS += \
    a_all_template_window.ui \
    a_list_template_window.ui \
    a_login_windows.ui \
    a_tasks_template_window.ui \
    b_main_window.ui \
    c_promanawindow.ui \
    c_promanawindow_newpro.ui \
    c_promanawindow_newpro_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
