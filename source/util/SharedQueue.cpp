#include "util/SharedQueue.h"

SharedQueue::SharedQueue() {}

SharedQueue::~SharedQueue() {}

bool SharedQueue::serverSend(Packet &&p)
{
    serverQueue.push(std::move(p));
    return true;
}

Packet::PacketPtr SharedQueue::serverReceive()
{
    return clientQueue.try_pop();
}

size_t SharedQueue::serverQueueSize() const
{
    return serverQueue.size();
}

bool SharedQueue::clientSend(Packet &&p)
{
    clientQueue.push(std::move(p));
    return true;
}

Packet::PacketPtr SharedQueue::clientReceive()
{
    return serverQueue.try_pop();
}

size_t SharedQueue::clientQueueSize() const
{
    return clientQueue.size();
}

void SharedQueue::setLocalGPSStatus(const LocalGPSStatus &status)
{
    std::lock_guard<std::mutex> lock(gpsMutex);
    gpsStatus = status;
    hasGPSStatus = true;
}

bool SharedQueue::getLocalGPSStatus(LocalGPSStatus &status) const
{
    std::lock_guard<std::mutex> lock(gpsMutex);
    if (!hasGPSStatus)
        return false;
    status = gpsStatus;
    return true;
}
