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
#include "ModbusASCII.h"
#include "BufferStream.h"
#include "LRC.h"

#include <format>

namespace ModbusOver
{
Buffer ModbusASCII::toADU(Buffer pdu) const
{
    pdu.prepend(m_slave);
    pdu.append(lrc(pdu.data().data(), static_cast<uint16_t>(pdu.size())));
    pdu.encodeASCII();
    pdu.prepend('\x3a');
    pdu.append('\x0d');
    pdu.append(m_asciiLF);
    return pdu;
}

std::optional<BufferStream> ModbusASCII::toPDU(FunctionCode expectFunctionCode, Buffer& adu) const
{
    if (adu.data().front() != '\x3A')
        return std::nullopt;
    adu.decodeASCII(1, adu.size() - 3);
    BufferStream stream(adu);
    uint8_t slave, receiveCode;
    stream >> slave;
    stream.peak(receiveCode);
    if (slave != m_slave) // 非请求从机回复
    {
        adu.data().clear();
        return std::nullopt;
    }
    checkException(expectFunctionCode, receiveCode, stream);
    if (lrc(adu.data().data(), static_cast<uint16_t>(adu.size() - 1)) != adu.data().back())
        throw std::exception("Response data lrc error.");
    return stream;
}

uint16_t ModbusASCII::aduMaximum() const { return 513; }

uint16_t ModbusASCII::minimumSize() const { return 9; }

} // namespace ModbusOver