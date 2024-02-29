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

#include "digishow.h"
#include "digishow_pixel_player.h"

DigishowPixelPlayer::DigishowPixelPlayer(QObject *parent) : QObject(parent)
{
    m_isLoaded = false;
    m_mediaType = DigishowPixelPlayer::MediaUnknown;

    m_fadein = 0;
    m_position = 0;
    m_duration = 0;
    m_volume = 1.0;
    m_speed = 1.0;
    m_repeat = false;

    // init video playback
    m_videoPlayer = new QMediaPlayer();
    m_videoSurface = new DppVideoSurface();
    connect(m_videoSurface, SIGNAL(frameReady(const QVideoFrame &)), this, SLOT(onVideoFrameReady(const QVideoFrame &)));
    m_videoPlayer->setVideoOutput(m_videoSurface);

    // init image sequence playback
    m_imageSequenceFPS = 10;
    m_imageSequenceCurrentFrame = -1;

    // init timing control
    m_timerPlayer = new QTimer();
    connect(m_timerPlayer, SIGNAL(timeout()), this, SLOT(onTimerPlayerFired()));

    m_timerFader = new QTimer();
    connect(m_timerFader, SIGNAL(timeout()), this, SLOT(onTimerFaderFired()));

    m_timerSequencer = new QTimer();
    connect(m_timerSequencer, SIGNAL(timeout()), this, SLOT(onTimerSequencerFired()));

    m_elapsed = new QElapsedTimer();
    m_elapsed->start();

    // init pixel frame buffer
    m_framePixelWidth = 0;
    m_framePixelHeight = 0;

}

DigishowPixelPlayer::~DigishowPixelPlayer()
{
    if (m_isPlaying) stop();

    delete m_videoPlayer;
    delete m_videoSurface;

    foreach (QImage *image, m_imageSequence) delete image;
    m_imageSequence.clear();

    delete m_timerPlayer;
    delete m_timerFader;
    delete m_timerSequencer;

    delete m_elapsed;
}

bool DigishowPixelPlayer::load(const QString &mediaUrl, int mediaType)
{
    if (m_isLoaded) return false;

    m_mediaType = mediaType;

    if (m_mediaType == MediaImage) {

        // load image
        QString filePath = AppUtilities::fileUrlPath(mediaUrl);
        if (filePath.isEmpty()) return false; // only support local file

        QImage image;
        if (!image.load(filePath)) return false;
        if (image.format() != QImage::Format_ARGB32)
            image = image.convertToFormat(QImage::Format_ARGB32);

        if (image.width() * image.height() * 4 == image.sizeInBytes())
            updateFrameBuffer(image.bits(), image.width(), image.height());

    } else if (m_mediaType == MediaVideo) {

        // load video
        QUrl url(mediaUrl);
        if (!url.isValid()) return false;

        QMediaContent content(url);
        if (content.isNull()) return false;
        m_videoPlayer->setMedia(content);

    } else if (m_mediaType == MediaImageSequence) {

        // load image sequence
        QString iniFilePath = AppUtilities::fileUrlPath(mediaUrl);
        if (iniFilePath.isEmpty()) return false; // only support local file

        // read settings
        QSettings settings(iniFilePath, QSettings::IniFormat);
        settings.setIniCodec("UTF-8");
        QString sequFile = settings.value("file").toString();
        int sequFirst = settings.value("first", 0).toInt();
        int sequLast  = settings.value("last", 1).toInt();
        int sequStep  = settings.value("step", 1).toInt();
        int sequFPS   = settings.value("fps", 10).toInt();
        if (sequFile.isEmpty()) return false;
        sequFPS = qMax(sequFPS, 1);
        sequFPS = qMin(sequFPS, 120);

        // load image frames
        QFileInfo fi(iniFilePath);
        QString dirPath = fi.absolutePath();

        for (int n=sequFirst ; n<=sequLast ; n+=sequStep) {
            QString imageFileName = QString().sprintf(sequFile.toUtf8().constData(), n);
            QString imageFilePath = QDir(dirPath).absoluteFilePath(imageFileName);
            QImage image;
            if (!image.load(imageFilePath)) continue;
            if (image.format() != QImage::Format_ARGB32)
                image = image.convertToFormat(QImage::Format_ARGB32);
            m_imageSequence.append(new QImage(image));
        }

        // set fps
        m_imageSequenceFPS = sequFPS;
    }


    m_isLoaded = true;
    return true;
}

void DigishowPixelPlayer::setVolume(double volume)
{
    m_volume = volume;

    if (m_mediaType == MediaVideo) m_videoPlayer->setVolume(volume*100);
}

void DigishowPixelPlayer::setSpeed(double speed)
{
    m_speed = speed;

    if (m_mediaType == MediaVideo) m_videoPlayer->setPlaybackRate(speed);
}

void DigishowPixelPlayer::play()
{
    if (!m_isLoaded) return;

    if (m_isPlaying) stop();
    m_isPlaying = true;

    m_elapsed->restart();

    m_frameBackup.fill(0x00);

    if (m_mediaType == MediaImage) {

        // start image playback

        // start fader timer
        m_timerFader->setInterval(20);
        m_timerFader->setSingleShot(false);
        m_timerFader->start();
        onTimerFaderFired();

    } else if (m_mediaType == MediaVideo) {

        // start video playback

        // start player timer
        if (m_duration > 0) {
            m_timerPlayer->setInterval(m_duration);
            m_timerPlayer->setSingleShot(!m_repeat);
            m_timerPlayer->start();
        }

        // start video player
        if (m_position > 0) m_videoPlayer->setPosition(m_position);
        m_videoPlayer->play();

    } else if (m_mediaType == MediaImageSequence) {

        // start image sequence playback

        // start player timer
        if (m_duration > 0) {
            m_timerPlayer->setInterval(m_duration);
            m_timerPlayer->setSingleShot(!m_repeat);
            m_timerPlayer->start();
        }

        // start sequencer timer
        m_imageSequenceCurrentFrame = -1;
        m_timerSequencer->setInterval(10);
        m_timerSequencer->setSingleShot(false);
        m_timerSequencer->start();
        onTimerSequencerFired();
    }
}

void DigishowPixelPlayer::stop()
{
    if (!m_isLoaded) return;

    if (m_mediaType == MediaImage) {

        // stop image plackback
        m_timerFader->stop();

    } else if (m_mediaType == MediaVideo) {

        // stop video playback
        m_videoPlayer->stop();
        m_timerPlayer->stop();

    } else if (m_mediaType == MediaImageSequence) {

        // stop image sequence playback
        m_timerSequencer->stop();
        m_timerPlayer->stop();
    }

    m_isPlaying = false;
}

void DigishowPixelPlayer::clearPixelMapping()
{
    m_pixelMappingList.clear();
}

int DigishowPixelPlayer::addPixelMapping(dppPixelMapping mapping)
{
    m_pixelMappingList.append(mapping);
    return m_pixelMappingList.length()-1; // index of the item added into the mapping list
}

int DigishowPixelPlayer::transferFrameAllMappedPixels()
{
    int count = 0;
    foreach (const dppPixelMapping mapping, m_pixelMappingList) {
        count += transferFramePixels(mapping);
    }
    return count;
}

void DigishowPixelPlayer::updateFrameBuffer(uint8_t *pFramePixels, int width, int height)
{
    int length = width * height * 4; // each pixel contains four bytes
    m_frameLatest = QByteArray((const char*)pFramePixels, length);

    if (m_frameBackup.isEmpty() || m_frameBackup.length() != length ||
        m_framePixelWidth != width || m_framePixelHeight != height) {
        m_frameBackup = QByteArray(length, 0);
    }

    m_framePixelWidth = width;
    m_framePixelHeight = height;

    emit frameUpdated();
}

int DigishowPixelPlayer::transferFramePixels(dppPixelMapping mapping)
{
    // pixel mode
    int rOffset, gOffset, bOffset;
    switch (mapping.pixelMode) {
    case PixelMono: rOffset = 0, gOffset = 0, bOffset = 0; break;
    case PixelRGB:  rOffset = 0; gOffset = 1; bOffset = 2; break;
    case PixelRBG:  rOffset = 0; gOffset = 2; bOffset = 1; break;
    case PixelGRB:  rOffset = 1; gOffset = 0; bOffset = 2; break;
    case PixelGBR:  rOffset = 2; gOffset = 0; bOffset = 1; break;
    case PixelBRG:  rOffset = 1; gOffset = 2; bOffset = 0; break;
    case PixelBGR:  rOffset = 2; gOffset = 1; bOffset = 0; break;
    default:
        return 0;
    }

    // copy mapped pixels
    uint8_t *pDataPixels = (uint8_t*)m_frameLatest.data();
    uint8_t *pDataBackup = (uint8_t*)m_frameBackup.data();
    int bytesPerPixelInDataOut = (mapping.pixelMode == PixelMono ? 1 : 3);

    uint8_t* pDataOut = mapping.pDataOut;
    int idxByteIn = 0;
    int idxByteOut = 0;
    int pixelMappedCount = 0;

    // inline pixel mapping parameters
    int xFrom = 0;
    int xTo = mapping.pixelCountX;
    int xStep = 1;

    // pixel lines mapping
    for (int y = 0 ; y < mapping.pixelCountY ; y++) {

        // confirm not out of the original frame height
        int originY = mapping.pixelOffsetY + (mapping.pixelSpacingY+1) * y;
        if (originY >= m_framePixelHeight) break;

        int idxByteInRow = originY * m_framePixelWidth * 4; // each original pixel contains four bytes

        // inline pixel mapping
        if (mapping.mappingMode == MappingByRowL2R ||
           (mapping.mappingMode == MappingByRowL2Z && (y % 2 == 0)) ||
           (mapping.mappingMode == MappingByRowR2Z && (y % 2 == 1))) {

            xFrom = 0;
            xTo   = mapping.pixelCountX;
            xStep = 1;
        } else
        if (mapping.mappingMode == MappingByRowR2L ||
           (mapping.mappingMode == MappingByRowL2Z && (y % 2 == 1)) ||
           (mapping.mappingMode == MappingByRowR2Z && (y % 2 == 0))) {

            xFrom = mapping.pixelCountX-1;
            xTo   = -1;
            xStep = -1;
        }

        //qDebug() << "y --------------------------- " << y;

        for (int x = xFrom ; x != xTo ; x += xStep) {

            //qDebug() << "x" << x;

            // confirm not out of the data output buffer
            if (pixelMappedCount+1 > mapping.dataInPixelOffset + mapping.dataOutPixelCount) break;
            pixelMappedCount++;

            // ignore unwanted leading pixels in the data input buffer
            if (mapping.dataInPixelOffset > 0 && pixelMappedCount <= mapping.dataInPixelOffset) continue;

            // confirm not out of the original frame width
            int originX = mapping.pixelOffsetX + (mapping.pixelSpacingX+1) * x;
            if (originX >= m_framePixelWidth) { idxByteOut += bytesPerPixelInDataOut; continue; }

            idxByteIn = idxByteInRow + originX * 4; // each original pixel contains four bytes

            // copy pixel bytes
            uint8_t opacity = pDataPixels[idxByteIn+3]; // get the alpha byte
            if (opacity != 0) {

                if (m_fadein > 0 && m_elapsed->elapsed() < m_fadein) {

                    // with fade-in effect

                    // backup origin pixels
                    if (pDataBackup[idxByteIn+3] == 0) {
                        pDataBackup[idxByteIn+3] = 0xff; // mark as backed up
                        pDataBackup[idxByteIn+2] = pDataOut[idxByteOut + rOffset];
                        pDataBackup[idxByteIn+1] = pDataOut[idxByteOut + gOffset];
                        pDataBackup[idxByteIn  ] = pDataOut[idxByteOut + bOffset];
                    }

                    float fadingInPercent  = (float)m_elapsed->elapsed() / m_fadein;
                    float fadingOutPercent = 1 - fadingInPercent;

                    // obtain new pixels
                    if (mapping.pixelMode == PixelMono) {
                        pDataOut[idxByteOut] = pDataPixels[idxByteIn]*fadingInPercent + pDataBackup[idxByteIn]*fadingOutPercent;
                    } else {
                        pDataOut[idxByteOut + rOffset] = pDataPixels[idxByteIn+2]*fadingInPercent + pDataBackup[idxByteIn+2]*fadingOutPercent;
                        pDataOut[idxByteOut + gOffset] = pDataPixels[idxByteIn+1]*fadingInPercent + pDataBackup[idxByteIn+1]*fadingOutPercent;
                        pDataOut[idxByteOut + bOffset] = pDataPixels[idxByteIn  ]*fadingInPercent + pDataBackup[idxByteIn  ]*fadingOutPercent;
                    }

                } else {

                    // without fade-in effect

                    // just copy new pixels
                    if (mapping.pixelMode == PixelMono) {
                        pDataOut[idxByteOut] = pDataPixels[idxByteIn];
                    } else {
                        pDataOut[idxByteOut + rOffset] = pDataPixels[idxByteIn+2];
                        pDataOut[idxByteOut + gOffset] = pDataPixels[idxByteIn+1];
                        pDataOut[idxByteOut + bOffset] = pDataPixels[idxByteIn  ];
                    }
                }
            }

            idxByteOut += bytesPerPixelInDataOut;
        }
    }

    return idxByteOut; // number of bytes transferred
}

void DigishowPixelPlayer::onVideoFrameReady(const QVideoFrame &frame)
{
    QVideoFrame f(frame);
    f.map(QAbstractVideoBuffer::ReadOnly);
    if (f.width() * f.height() * 4 == f.mappedBytes())
        updateFrameBuffer(f.bits(), f.width(), f.height());
    f.unmap();
}

void DigishowPixelPlayer::onTimerPlayerFired()
{
    if (m_mediaType == MediaVideo) {

        m_videoPlayer->stop();

        if (m_repeat) {
            m_elapsed->restart();
            m_frameBackup.fill(0x00);
            if (m_position > 0) m_videoPlayer->setPosition(m_position);
            m_videoPlayer->play();
        }

    } else if (m_mediaType == MediaImageSequence) {

        m_timerSequencer->stop();

        if (m_repeat) {
            m_elapsed->restart();
            m_frameBackup.fill(0x00);
            m_imageSequenceCurrentFrame = -1;
            m_timerSequencer->start();
            onTimerSequencerFired();
        }
    }
}

void DigishowPixelPlayer::onTimerFaderFired()
{
    if (m_elapsed->hasExpired(m_fadein)) {
        m_timerFader->stop();
        m_isPlaying = false;
    }

    emit frameUpdated();
}

void DigishowPixelPlayer::onTimerSequencerFired()
{
    int currentFrame = (m_position + m_elapsed->elapsed() * m_speed) * m_imageSequenceFPS / 1000;
    if (currentFrame == m_imageSequenceCurrentFrame) return;
    if (currentFrame >= m_imageSequence.length()) return;

    QImage *image = m_imageSequence[currentFrame];
    m_imageSequenceCurrentFrame = currentFrame;

    if (image->width() * image->height() * 4 == image->sizeInBytes())
        updateFrameBuffer(image->bits(), image->width(), image->height());
}
