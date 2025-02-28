/*
    Copyright 2021 Robin Zhang & Labs

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtWebEngine/qtwebengineglobal.h>
#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QFont>
#include <QScreen>
#include <QDebug>
//#include <QQmlComponent>
//#include <QQuickWindow>

#include "QtQrCodeQuickItem.hpp"

#include "digishow.h"
#include "digishow_environment.h"
#include "digishow_interface.h"
#include "digishow_slot.h"
#include "digishow_data.h"
#include "digishow_metronome.h"
#include "digishow_pixel_player.h"
#include "digishow_remote_web.h"


#ifdef Q_OS_MAC
#include <CoreMIDI/CoreMIDI.h>
#endif

#ifdef DIGISHOW_CLOUD
#include "digishow_cloud.h"
#else
#include "digishow_cloud_dummy.h"
#endif

#ifdef DIGISHOW_EXPERIMENTAL
#include "digishow_experimental.h"
#endif

class MyApplication : public QGuiApplication
{
public:
    MyApplication(int &argc, char **argv)
        : QGuiApplication(argc, argv)
    {
        m_qmlRoot = nullptr;
    }

    void setQmlRoot(QObject *qmlRoot) { m_qmlRoot = qmlRoot; }

    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::FileOpen) {

            QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
            QString filepath = openEvent->file();

            //g_app->loadFile(filepath);

            // call function in qml root object
            if (m_qmlRoot != nullptr) {
                QVariant r;
                QMetaObject::invokeMethod(
                            m_qmlRoot, "fileOpen", Qt::DirectConnection,
                            Q_RETURN_ARG(QVariant, r), Q_ARG(QVariant, filepath));
            }

        } else if (event->type() == QEvent::Close) {

            g_app->stop();

            event->ignore();

            // call function in qml root object
            if (m_qmlRoot != nullptr) {
                QVariant r;
                QMetaObject::invokeMethod(
                            m_qmlRoot, "appClose", Qt::DirectConnection,
                            Q_RETURN_ARG(QVariant, r));
            }
        }

        return QGuiApplication::event(event);
    }

private:
    QObject *m_qmlRoot;
};

QString fontAvailable(const QStringList &fontNames)
{
    QFontDatabase fonts;
    for (int n=0 ; n<fontNames.length() ; n++) {
        QString fontName = fontNames[n];
        if (fonts.families().contains(fontName)) return fontName;
    }
    return QString();
}

int main(int argc, char *argv[])
{

#ifdef Q_OS_MAC

    // due to a known issue of rtmidi on macos
    // we need have to create a coremidi client when starting the app

    MIDIClientRef midiclient;
    OSStatus status;
    status = MIDIClientCreate(CFSTR("DgsMidiClient"), NULL, NULL, &midiclient);
    if (status != noErr) {
        printf("Error trying to create MIDI Client structure: %d\n", status);
        exit(status);
    }
#endif

    qRegisterMetaType<dgsSignalData>();

    qmlRegisterType<AppUtilities>       ("DigiShow", 1, 0, "AppUtilities");
    qmlRegisterType<DigishowEnvironment>("DigiShow", 1, 0, "DigishowEnvironment");
    qmlRegisterType<DigishowApp>        ("DigiShow", 1, 0, "DigishowApp");
    qmlRegisterType<DigishowInterface>  ("DigiShow", 1, 0, "DigishowInterface");
    qmlRegisterType<DigishowSlot>       ("DigiShow", 1, 0, "DigishowSlot");
    qmlRegisterType<DigishowData>       ("DigiShow", 1, 0, "DigishowData");
    qmlRegisterType<DigishowMetronome>  ("DigiShow", 1, 0, "DigishowMetronome");
    qmlRegisterType<DigishowPixelPlayer>("DigiShow", 1, 0, "DigishowPixelPlayer");
    qmlRegisterType<DigishowRemoteWeb>  ("DigiShow", 1, 0, "DigishowRemoteWeb");
    qmlRegisterType<DigishowCloud>      ("DigiShow", 1, 0, "DigishowCloud");
    qmlRegisterType<QtQrCodeQuickItem>  ("DigiShow", 1, 0, "QrCode");

    QCoreApplication::setOrganizationName("robinz");
    QCoreApplication::setOrganizationDomain("robinz.org");
    QCoreApplication::setApplicationName(g_appname);
    QCoreApplication::setApplicationVersion(g_version);

    // load app options
    QVariantMap appOptions = DigishowEnvironment().getAppOptions();

    // set app attributes
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    bool hidpi = appOptions.value("hidpi", true).toBool();
    if (hidpi) QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    float scale = appOptions.value("scale", 0).toFloat();
    if (scale > 0) qputenv("QT_SCALE_FACTOR", QString::number(scale).toUtf8());

#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
    QtWebEngine::initialize();
#endif

    // create app
    MyApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

#if (QT_VERSION < QT_VERSION_CHECK(5,15,0))
    QtWebEngine::initialize();
#endif

    // set translation
    QString appLanguage = appOptions.value("language").toString();

    if (appLanguage.isEmpty()) {
        QLocale locale = QLocale::system();
        if (locale.uiLanguages().length() > 0 &&
            locale.uiLanguages()[0].startsWith("zh") &&
            locale.uiLanguages()[0].endsWith("CN"))
            appLanguage = "zh_CN";
    }

    QTranslator qtTranslator;
    if (!appLanguage.isEmpty())
        if (qtTranslator.load(":translations/language." + appLanguage + ".qm"))
            app.installTranslator(&qtTranslator);

    // set app font
    QString appFontName = appOptions.value("font").toString();
    if (appFontName.isEmpty()) {
        appFontName = "Arial";
        if (appLanguage == "zh_CN") {

#ifdef Q_OS_WIN
            QString fontName = fontAvailable({"Microsoft YaHei Light", "微软雅黑 Light", "Microsoft YaHei", "微软雅黑"});
            if (!fontName.isEmpty()) appFontName = fontName;
#endif
#ifdef Q_OS_MAC
            QString fontName = fontAvailable({"Yuanti SC"});


            if (!fontName.isEmpty()) appFontName = fontName;
#endif
        } else if (appLanguage == "jp") {
#ifdef Q_OS_WIN
            QString fontName = fontAvailable({"Yu Gothic UI", "MS UI Gothic", "MS Gothic"});
            if (!fontName.isEmpty()) appFontName = fontName;
#endif
        }
    }
    app.setFont(QFont(appFontName));

    // start main app
    g_app = new DigishowApp();

#ifdef DIGISHOW_EXPERIMENTAL
    DigishowExperimental ex(&app);
#endif

#ifndef DIGISHOW_NON_GUI

    // start main ui
    QQmlApplicationEngine engine;

    /*
    QQmlComponent component(&engine);
    component.loadUrl(QUrl("qrc:///Splash.qml"));
    if (component.isReady()) component.create();
    */

    const QUrl url(QStringLiteral("qrc:///MainWindow.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    app.setQmlRoot(engine.rootObjects()[0]);

    // window mode
    int windowMode = appOptions.value("window", 1).toInt();
    switch (windowMode) {
    case 0: app.allWindows().first()->showMinimized(); break;
    case 2: app.allWindows().first()->showFullScreen(); break;
    }

#endif

    // enable autostart
    g_app->enableAutostart(appOptions.value("autostart", false).toBool());

    // load data file
    if (argc > 1) {
        QString filepath = QString::fromLocal8Bit(argv[1]);
        g_app->loadFile(filepath);
    }

#ifdef DIGISHOW_EXPERIMENTAL
    ex.initExperimentalApp(argc, argv);
#endif

    // start app runloop
    int r = app.exec();
    delete(g_app);
    return r;

}
