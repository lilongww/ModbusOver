/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023 LiLong                                                   **
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
#include "ModbusException.h"

#include <map>

namespace ModbusOver
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

/*!
    \class      ModbusOver::ModbusException
    \brief      Modbus异常类，报告从机返回的错误.
    \inherits   std::runtime_error
    \ingroup    ModbusOver
    \inmodule   ModbusOver
    \inheaderfile ModbusException.h
*/

/*!
    \fn         inline ModbusOver::ModbusException::ModbusException(ModbusOver::ExceptionCode code)
    \brief      构造函数 \a code.
*/

/*!
    \fn         inline ModbusOver::ExceptionCode ModbusOver::ModbusException::code() const
    \brief      返回Modbus异常码.
*/

/*!
    \brief      将Modbus异常码 \a code 转换为文字.
*/
const std::string& ModbusException::toString(ExceptionCode code)
{
    if (auto it = ExceptionCodeTextMap.find(code); it != ExceptionCodeTextMap.end())
    {
        return it->second;
    }
    return Unknown;
}
} // namespace ModbusOver