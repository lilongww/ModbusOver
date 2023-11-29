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
#include "ModbusException.h"

#include <map>

namespace ModbusCpp
{
static const std::map<ExceptionCode, std::string> ExceptionCodeTextMap {
    { ExceptionCode::IllegalFunction, "Illegal function." },
    { ExceptionCode::IllegalDataAddress, "Illegal data address." },
    { ExceptionCode::IllegalDataValue, "Illegal data value." },
    { ExceptionCode::ServerDeviceFailure, "Server device failure." },
    { ExceptionCode::Acknowledge, "Acknowledge." },
    { ExceptionCode::ServerDeviceBusy, "Server device busy." },
    { ExceptionCode::MemoryParityError, "Memory parity error." },
    { ExceptionCode::GatewayPathUnavailable, "Gateway path unavailable." },
    { ExceptionCode::GatewayTargetDeviceFailedToRespond, "Gateway target device failed to respond." }
};
static const std::string Unknown = "Unknown exception code.";
const std::string& ModbusException::toString(ExceptionCode code)
{
    if (auto it = ExceptionCodeTextMap.find(code); it != ExceptionCodeTextMap.end())
    {
        return it->second;
    }
    return Unknown;
}
} // namespace ModbusCpp