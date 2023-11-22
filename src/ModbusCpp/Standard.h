/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023 LiLong                                                   **
**  This file is part of ModbusCpp.                                             **
**                                                                              **
**  ModbusCpp is free software: you can redistribute it and/or modify           **
**  it under the terms of the GNU Lesser General Public License as published by **
**  the Free Software Foundation, either version 3 of the License, or           **
**  (at your option) any later version.                                         **
**                                                                              **
**  ModbusCpp is distributed in the hope that it will be useful,                **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of              **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               **
**  GNU Lesser General Public License for more details.                         **
**                                                                              **
**  You should have received a copy of the GNU Lesser General Public License    **
**  along with ModbusCpp.  If not, see <https://www.gnu.org/licenses/>.         **
**********************************************************************************/

#include <cstdint>

namespace ModbusCpp
{
static constexpr uint8_t SlaveMin         = 1;
static constexpr uint8_t SlaveMax         = 247;
static constexpr uint8_t SlaveReservedMin = 248;
static constexpr uint8_t SlaveReservedMax = 255;
static constexpr uint8_t Broadcast        = 0;

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

} // namespace ModbusCpp