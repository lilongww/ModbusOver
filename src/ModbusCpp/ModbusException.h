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
#pragma once

#include "Standard.h"
#include "modbuscpp_global.h"

#include <stdexcept>

namespace ModbusCpp
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4275)
#endif
class MODBUSCPP_EXPORT ModbusException : public std::runtime_error
{
public:
    inline ModbusException(ExceptionCode code) : runtime_error(toString(code)), m_code(code) {}
    inline ExceptionCode code() const { return m_code; }
    static const std::string& toString(ExceptionCode code);

private:
    ExceptionCode m_code;
};
#ifdef _MSC_VER
#pragma warning(pop)
#endif
} // namespace ModbusCpp