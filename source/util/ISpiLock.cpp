#include "util/ISpiLock.h"
#include "util/ILog.h"

namespace
{
ISpiLock *hostLock = nullptr;
}

void ISpiLock::install(ISpiLock *lock)
{
    // A create() overload that was given no lock must not clear one an earlier call
    // installed: drivers and SD code constructed by that earlier call keep guarding
    // against it, so silently dropping it here would un-serialize the bus.
    if (!lock)
        return;

    if (hostLock && hostLock != lock)
        ILOG_WARN("ISpiLock: replacing the installed bus lock");

    hostLock = lock;
}

ISpiLock *ISpiLock::installed(void)
{
    return hostLock;
}
