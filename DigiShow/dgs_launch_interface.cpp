#include "dgs_launch_interface.h"

DgsLaunchInterface::DgsLaunchInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "launch";
}

DgsLaunchInterface::~DgsLaunchInterface()
{
    closeInterface();
}

int DgsLaunchInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsLaunchInterface::closeInterface()
{
    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsLaunchInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    int channel = m_endpointInfoList[endpointIndex].channel;
    g_app->startLaunch("launch" + QString::number(channel));

    return ERR_NONE;
}
