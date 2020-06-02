#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <MSWSock.h>
#include <mstcpip.h>
#include <memory.h>
#include <cstdint>
#include <process.h>

#include "enviroment.h"
#include "macro.h"

#include "concurrency/tls.h"
#include "concurrency/MemoryPoolTLS.h"
#include "concurrency/MPSCQueue.h"

#include "util/exception.h"
#include "util/TimeScheduler.h"
#include "util/dump.h"

#include "network/Packet.h"

#include "packet_handler.h"

#pragma comment (lib, "ws2_32")

