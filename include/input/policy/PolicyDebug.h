#pragma once

#include "util/ILog.h"

#ifdef MUI_POLICY_DEBUG
#define POLICY_DEBUG ILOG_DEBUG(__VA_ARGS__) 
#else
#define POLICY_DEBUG(...)
#endif
