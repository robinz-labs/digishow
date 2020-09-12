#ifndef DGSPIPEINTERFACE_H
#define DGSPIPEINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class DgsPipeInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsPipeInterface(QObject *parent = nullptr);
    ~DgsPipeInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;
};


#endif // DGSPIPEINTERFACE_H
