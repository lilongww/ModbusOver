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
#include "ModbusTCP.h"
#include "BufferStream.h"

namespace ModbusOver
{
constexpr auto MBAPSize = 7;

Buffer ModbusTCP::toADU(Buffer pdu) const
{
    MBAP mbap;
    mbap.transactionIdentifier = const_cast<ModbusTCP*>(this)->m_transactionIdentifier++;
    mbap.protocolIdentifier    = 0; // always 0
    mbap.length                = static_cast<uint8_t>(sizeof(mbap.unitIdentifier) + pdu.size());
    mbap.unitIdentifier        = m_slave;
    pdu.prepend(mbap.unitIdentifier);
    pdu.prepend(mbap.length);
    pdu.prepend(mbap.protocolIdentifier);
    pdu.prepend(mbap.transactionIdentifier);
    return pdu;
}

std::optional<BufferStream> ModbusTCP::toPDU(FunctionCode expectFunctionCode, Buffer& adu) const
{
    if (adu.size() < minimumSize())
        return std::nullopt;
    BufferStream stream(adu);
    MBAP mbap;
    stream >> mbap.transactionIdentifier >> mbap.protocolIdentifier >> mbap.length >> mbap.unitIdentifier;
    if (stream.size() < mbap.length - 1)
        return std::nullopt;
    if (mbap.transactionIdentifier != m_transactionIdentifier - 1) // 回复id不一致
    {
        adu.data().clear();
        return std::nullopt;
    }
    if (mbap.unitIdentifier != m_slave) // 非请求从机回复
    {
        adu.data().clear();
        return std::nullopt;
    }
    uint8_t receiveCode;
    stream.peak(receiveCode);
    checkException(expectFunctionCode, receiveCode, stream);
    return stream;
}

uint16_t ModbusTCP::aduMaximum() const { return 260; }

// MBAP size + functionCode size
uint16_t ModbusTCP::minimumSize() const { return MBAPSize + 1; }

} // namespace ModbusOver
