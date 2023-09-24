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

#ifndef DIGISHOWPIXELPLAYER_H
#define DIGISHOWPIXELPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QImage>
#include <QTimer>
#include <QElapsedTimer>
#include <QAbstractVideoSurface>

typedef struct dppPixelMapping {

    int pixelMode;
    int pixelCount;
    int pixelOffset;
    uint8_t* pDataOut;

    // defaults
    dppPixelMapping() :
      pixelMode(0),
      pixelCount(0),
      pixelOffset(0),
      pDataOut(nullptr)
    {}

} dppPixelMapping;


class DppVideoSurface;

class DigishowPixelPlayer : public QObject
{
    Q_OBJECT
public:

    enum PixelMode {
        PixelUnknown = 0,
        PixelMono,
        PixelRGB,
        PixelRBG,
        PixelGRB,
        PixelGBR,
        PixelBRG,
        PixelBGR
    };

    enum MediaType {
        MediaUnknown = 0,
        MediaImage,
        MediaVideo,
        MediaImageSequence
    };

    static int pixelMode(const QString &pixelMode) {
        if      (pixelMode == "mono") return PixelMono;
        else if (pixelMode == "rgb" ) return PixelRGB;
        else if (pixelMode == "rbg" ) return PixelRBG;
        else if (pixelMode == "grb" ) return PixelGRB;
        else if (pixelMode == "gbr" ) return PixelGBR;
        else if (pixelMode == "brg" ) return PixelBRG;
        else if (pixelMode == "bgr" ) return PixelBGR;
        else return PixelUnknown;
    }

    explicit DigishowPixelPlayer(QObject *parent = nullptr);
    ~DigishowPixelPlayer();

    bool load(const QString &mediaUrl, int mediaType);

    void setFadeIn(int msec) { m_fadein = msec; }
    void setPosition(int msec) { m_position = msec; }
    void setDuration(int msec) { m_duration = msec; }
    void setRepeat(bool repeat) { m_repeat = repeat; }
    void setVolume(double volume); // 0 ~ 1.0
    void setSpeed(double speed);   // 0 ~ 4.0
    void play();
    void stop();

    bool isLoaded() { return m_isLoaded; }
    bool isPlaying() { return m_isPlaying; }

    void clearPixelMapping();
    int  addPixelMapping(dppPixelMapping mapping);
    int  transferFrameAllMappedPixels();

signals:
    void frameUpdated();

private slots:
    void onVideoFrameReady(const QVideoFrame &frame);
    void onTimerPlayerFired();
    void onTimerFaderFired();
    void onTimerSequencerFired();

private:
    int m_mediaType;

    bool m_isLoaded;
    bool m_isPlaying;

    int m_fadein;    // millisecond
    int m_position;  // millisecond
    int m_duration;  // millisecond
    double m_volume; // 0 ~ 1.0
    double m_speed;  // 0 ~ 4.0
    bool m_repeat;

    // video playback
    QMediaPlayer *m_videoPlayer;
    DppVideoSurface *m_videoSurface;

    // image sequence playback
    QList<QImage*> m_imageSequence;
    int m_imageSequenceFPS;
    int m_imageSequenceCurrentFrame;

    // timing control
    QTimer *m_timerPlayer;    // for video and image sequence
    QTimer *m_timerFader;     // for image
    QTimer *m_timerSequencer; // for image sequence

    QElapsedTimer *m_elapsed;

    // pixel frame buffer
    QByteArray m_frameLatest;  // each pixel occupies 4 bytes BGRA (A is alpha, used as opacity mask, 0 = transparency, other value = opacity)
    QByteArray m_frameBackup;  // each pixel occupies 4 bytes BGRX (X is the marker for backup, 0 = not backed up, 0xff = backed up)

    // pixel mapping
    QList<dppPixelMapping> m_pixelMappingList;

    void updateFrameBuffer(uint8_t *pFramePixels, int length);
    int transferFramePixels(dppPixelMapping mapping);
};

class DppVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const
    {
        Q_UNUSED(handleType);
        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_ARGB32;
    }

    bool present(const QVideoFrame &frame)
    {
        if (!frame.isValid()) return false;
        emit frameReady(frame);
        return true;
    }

    QByteArray &framePixels(){ return m_frameLatest; }

signals:
    void frameReady(const QVideoFrame &frame);

private:

    QByteArray m_frameLatest;
};


#endif // DIGISHOWPIXELPLAYER_H
