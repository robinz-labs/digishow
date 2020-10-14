#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include <QtQml/QQmlComponent>
//#include <QtQuick/QQuickWindow>
#include <QtWebEngine/qtwebengineglobal.h>
#include <QFileOpenEvent>
#include <QFont>
#include <QDebug>
#include "digishow.h"
#include "digishow_environment.h"
#include "digishow_interface.h"
#include "digishow_slot.h"

#ifdef Q_OS_MAC
#include <CoreMIDI/CoreMIDI.h>
#endif

Q_DECLARE_METATYPE(dgsSignalData)

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

    qmlRegisterType<AppUtilities>("DigiShow", 1, 0, "AppUtilities");
    qmlRegisterType<DigishowEnvironment>("DigiShow", 1, 0, "DigishowEnvironment");
    qmlRegisterType<DigishowApp>("DigiShow", 1, 0, "DigishowApp");
    qmlRegisterType<DigishowInterface>("DigiShow", 1, 0, "DigishowInterface");
    qmlRegisterType<DigishowSlot>("DigiShow", 1, 0, "DigishowSlot");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setOrganizationName("robinz");
    QCoreApplication::setOrganizationDomain("robinz.org");
    QCoreApplication::setApplicationName(g_appname);
    QCoreApplication::setApplicationVersion(g_version);

    MyApplication app(argc, argv);
    app.setFont(QFont("Arial"));
    app.setQuitOnLastWindowClosed(false);

    QtWebEngine::initialize();

    // load translation
    QString appLanguage = DigishowEnvironment().getAppOptions().value("language").toString();
    QTranslator qtTranslator;
    if (!appLanguage.isEmpty())
        if (qtTranslator.load(":translations/language." + appLanguage + ".qm"))
            app.installTranslator(&qtTranslator);

    // start main app
    g_app = new DigishowApp();

    // start main ui
    QQmlApplicationEngine engine;

    //QQmlComponent component(&engine);
    //component.loadUrl(QUrl("qrc:///Splash.qml"));
    //if (component.isReady()) component.create();

    const QUrl url(QStringLiteral("qrc:///MainWindow.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    app.setQmlRoot(engine.rootObjects()[0]);

    // load data file
    if (argc > 1) {
        QString filepath = argv[1];
        if (g_app->loadFile(filepath)) g_app->start();
    }

    // start app runloop
    int r = app.exec();
    delete(g_app);
    return r;

}
