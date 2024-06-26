﻿/*********************************************************************************
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

#include <chrono>
#include <cstdint>

namespace ModbusOver
{
class ProtocolDebug;
struct MasterCommonData
{
    std::chrono::milliseconds timeout { 5000LL };
    uint8_t slave { 0xFF };
    bool useBigendianCRC16 = false;
    std::chrono::milliseconds rtsDelay { 0LL };
    char modbusAsciiLF = '\x0A';
    std::shared_ptr<ProtocolDebug> protoDebug;
};
} // namespace ModbusOver