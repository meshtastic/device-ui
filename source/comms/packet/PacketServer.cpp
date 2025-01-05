#include "PacketServer.h"
#include "SharedQueue.h"
#include <assert.h>

const uint32_t max_packet_queue_size = 200;

SharedQueue *sharedQueue = nullptr;

PacketServer *packetServer = nullptr;

PacketServer::PacketServer() : queue(nullptr) {}

PacketServer *PacketServer::init(void)
{
    packetServer = new PacketServer;
    sharedQueue = new SharedQueue;
    packetServer->begin(sharedQueue);
    return packetServer;
}

void PacketServer::begin(SharedQueue *_queue)
{
    queue = _queue;
}

Packet::PacketPtr PacketServer::receivePacket(void)
{
    assert(queue);
    if (queue->clientQueueSize() == 0)
        return {nullptr};
    return queue->serverReceive();
}

bool PacketServer::sendPacket(Packet &&p)
{
    assert(queue);
    if (queue->serverQueueSize() >= max_packet_queue_size) {
        return false;
    }
    queue->serverSend(std::move(p));
    return true;
}

bool PacketServer::hasData() const
{
    assert(queue);
    return queue->clientQueueSize() > 0;
}

bool PacketServer::available() const
{
    assert(queue);
    return queue->serverQueueSize() < max_packet_queue_size;
}
