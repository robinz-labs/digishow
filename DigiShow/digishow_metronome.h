#ifndef DIGISHOWMETRONOME_H
#define DIGISHOWMETRONOME_H

#include <QObject>
#include <QVariantMap>
#include <QThread>
#include <QMutex>
#include <QAudioOutput>
#include <QScopedPointer>
#include <QTimer>
#include <QElapsedTimer>

namespace ableton {
    class Link;
}

class DigishowMetronomeThread;
class ToneGenerator;

class DigishowMetronome : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning WRITE setRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool isSoundEnabled READ isSoundEnabled WRITE setSoundEnabled NOTIFY isSoundEnabledChanged)
    Q_PROPERTY(bool isLinkEnabled READ isLinkEnabled WRITE setLinkEnabled NOTIFY isLinkEnabledChanged)
    Q_PROPERTY(double bpm READ bpm WRITE setBpm NOTIFY bpmChanged)
    Q_PROPERTY(int quantum READ quantum WRITE setQuantum NOTIFY quantumChanged)

    Q_PROPERTY(double beat READ beat NOTIFY beatChanged)
    Q_PROPERTY(double phase READ phase NOTIFY beatChanged)

    Q_PROPERTY(int tapCount READ tapCount NOTIFY tapCountChanged)

public:
    explicit DigishowMetronome(QObject *parent = nullptr);
    ~DigishowMetronome();

    Q_INVOKABLE void reset();
    Q_INVOKABLE void setParameters(const QVariantMap &params);
    Q_INVOKABLE QVariantMap getParameters();

    Q_INVOKABLE bool isRunning() { return m_running; }
    Q_INVOKABLE void setRunning(bool running);
    Q_INVOKABLE bool isSoundEnabled() { return m_soundEnabled; }
    Q_INVOKABLE void setSoundEnabled(bool enabled);
    Q_INVOKABLE bool isLinkEnabled() { return m_linkEnabled; }
    Q_INVOKABLE void setLinkEnabled(bool enabled);

    Q_INVOKABLE double bpm() { return m_bpm; }
    Q_INVOKABLE void setBpm(double bpm);
    Q_INVOKABLE int quantum() { return m_quantum; }
    Q_INVOKABLE void setQuantum(int quantum);

    Q_INVOKABLE double beat() { return m_beat; }
    Q_INVOKABLE double phase() { return m_phase; }

    Q_INVOKABLE void tap();
    Q_INVOKABLE int tapCount() { return m_tapCount; }


    void run(); // called by the worker thread

signals:

    void isRunningChanged();
    void isSoundEnabledChanged();
    void isLinkEnabledChanged();
    void bpmChanged();
    void quantumChanged();

    void beatChanged();     // one beat progressed
    void quarterChanged();  // 1/4 beat progressed

    void tapCountChanged();

public slots:
    void onBeatChanged();
    void onQuarterChanged();

    void onTapTimeout();

private:

    ableton::Link *m_link;

    // parameters
    bool m_running;
    bool m_soundEnabled;
    bool m_linkEnabled;
    double m_bpm;
    int m_quantum;

    // runtime data
    double m_beat;
    double m_phase;

    // workder thread
    DigishowMetronomeThread *m_thread;
    QMutex m_mutex;

    // sound player
    QScopedPointer<ToneGenerator> m_soundGenerator;
    QScopedPointer<QAudioOutput> m_soundOutput;

    void startSoundOutput();
    void stopSoundOutput();

    // tap to bpm
    int m_tapCount;
    QElapsedTimer m_tapElapsedTimer;
    QTimer m_tapTimeoutTimer;
};

class DigishowMetronomeThread : public QThread
{
    Q_OBJECT
public:

    explicit DigishowMetronomeThread(QObject *parent) : QThread(parent) {
        m_metronome = (DigishowMetronome*)parent;
    }

private:
    DigishowMetronome* m_metronome;

    void run() {
        m_metronome->run();
    }
};

class ToneGenerator : public QIODevice
{
    Q_OBJECT

public:
    ToneGenerator(const QAudioFormat &format, qint64 durationUs, int sampleRate);

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;

private:
    void generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate);

private:
    qint64 m_pos = 0;
    QByteArray m_buffer;
};

#endif // DIGISHOWMETRONOME_H
