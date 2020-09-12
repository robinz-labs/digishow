#ifndef DGSARTNETINTERFACE_H
#define DGSARTNETINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class DgsArtnetInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsArtnetInterface(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DGSARTNETINTERFACE_H
