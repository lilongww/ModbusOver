/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023-2024 LiLong                                              **
**  This file is part of ModbusOver.                                            **
**                                                                              **
**  ModbusOver is free software: you can redistribute it and/or modify          **
**  it under the terms of the GNU Lesser General Public License as published by **
**  the Free Software Foundation, either version 3 of the License, or           **
**  (at your option) any later version.                                         **
**                                                                              **
**  ModbusOver is distributed in the hope that it will be useful,               **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of              **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               **
**  GNU Lesser General Public License for more details.                         **
**                                                                              **
**  You should have received a copy of the GNU Lesser General Public License    **
**  along with ModbusOver.  If not, see <https://www.gnu.org/licenses/>.        **
**********************************************************************************/
#pragma once

#include <cstdint>
#include <vector>

namespace ModbusOver
{
static constexpr uint8_t SlaveMin            = 1;
static constexpr uint8_t SlaveMax            = 247;
static constexpr uint8_t SlaveReservedMin    = 248;
static constexpr uint8_t SlaveReservedMax    = 255;
static constexpr uint8_t Broadcast           = 0;
static constexpr uint8_t ExceptionCodeAddend = 0x80;

enum class ModbusProtocol
{
    ModbusTCP,
    ModbusRTU,
    ModbusASCII
};

enum class AddressType
{
    TCP,
    UDP,
    SerialPort
};

enum class ExceptionCode : uint8_t
{
    IllegalFunction                    = 0x01,
    IllegalDataAddress                 = 0x02,
    IllegalDataValue                   = 0x03,
    ServerDeviceFailure                = 0x04,
    Acknowledge                        = 0x05,
    ServerDeviceBusy                   = 0x06,
    MemoryParityError                  = 0x08,
    GatewayPathUnavailable             = 0x0A,
    GatewayTargetDeviceFailedToRespond = 0x0B
};

struct CommEventCounter
{
    uint16_t status;
    uint16_t eventCount;
};

struct CommEventLog
{
    uint16_t status;
    uint16_t eventCount;
    uint16_t messageCount;
    std::vector<uint8_t> events;
};

} // namespace ModbusOver