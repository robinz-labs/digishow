#include "dgs_pipe_interface.h"

DgsPipeInterface::DgsPipeInterface(QObject *parent) : DigishowInterface(parent)
{
    m_interfaceOptions["type"] = "pipe";
}

DgsPipeInterface::~DgsPipeInterface()
{
    closeInterface();
}

int DgsPipeInterface::openInterface()
{
    if (m_isInterfaceOpened) return ERR_DEVICE_BUSY;

    updateMetadata();

    m_isInterfaceOpened = true;
    return ERR_NONE;
}

int DgsPipeInterface::closeInterface()
{
    m_isInterfaceOpened = false;
    return ERR_NONE;
}

int DgsPipeInterface::sendData(int endpointIndex, dgsSignalData data)
{
    int r = DigishowInterface::sendData(endpointIndex, data);
    if ( r != ERR_NONE) return r;

    emit dataReceived(endpointIndex, data);

    return ERR_NONE;
}
