#include "graphics/map/AsyncTileService.h"

AsyncTileService::AsyncTileService(ITileService *wrapped) : ITileService(""), wrapped_(wrapped)
{
    loader_.start(wrapped_);
}

AsyncTileService::~AsyncTileService()
{
    loader_.stop();
    loader_.flushQueues();
    delete wrapped_;
}

lv_image_dsc_t *AsyncTileService::loadRaw(const char *name)
{
    return wrapped_ ? wrapped_->loadRaw(name) : nullptr;
}

void AsyncTileService::loadAsync(uint32_t hash, uint32_t generation, const char *filename)
{
    loader_.enqueue(hash, generation, filename);
}

void AsyncTileService::tick(AsyncResultConsumer consumer)
{
    loader_.drainResults([&consumer](AsyncTileLoader::Result &r) { consumer(r.hash, r.generation, r.img_dsc); });
}

void AsyncTileService::resetAsync()
{
    loader_.flushQueues();
}
