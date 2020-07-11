#pragma once
#include <WinSock2.h>
#include <sqltypes.h>
#include <sqlext.h>

#include <Windows.h>
#include <MSWSock.h>
#include <mstcpip.h>
#include <memory.h>
#include <cstdint>
#include <process.h>


#include "core/enviroment.h"
#include "core/macro.h"
#include "event_context.h"

#include "concurrency/tls.h"
#include "concurrency/MPSCQueue.h"
//#include "concurrency/MemoryPoolTLS.h"

#include "util/exception.h"
//#include "concurrency/MemoryPool_Queue.h"
//#include "util/TimeScheduler.h"
//#include "util/dump.h"

#include "core/Packet.h"
#include "util/TimeScheduler.h"
#include "core/OuterServer.h"
#include "core/packet_handler.h"

#pragma comment (lib, "ws2_32")

