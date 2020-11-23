#ifndef DGSLAUNCHINTERFACE_H
#define DGSLAUNCHINTERFACE_H

#include <QObject>
#include "digishow_interface.h"

class DgsLaunchInterface : public DigishowInterface
{
    Q_OBJECT
public:
    explicit DgsLaunchInterface(QObject *parent = nullptr);
    ~DgsLaunchInterface() override;

    int openInterface() override;
    int closeInterface() override;
    int sendData(int endpointIndex, dgsSignalData data) override;
};


#endif // DGSLAUNCHINTERFACE_H
