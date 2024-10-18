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
#include "ModbusRTU.h"
#include "Buffer.h"
#include "BufferStream.h"
#include "CRC.h"

namespace ModbusOver
{
Buffer ModbusRTU::toADU(Buffer pdu) const
{
    pdu.prepend(m_slave);
    if (m_useBigendianCRC16)
        pdu.append(crc16(pdu.data().data(), static_cast<uint16_t>(pdu.data().size())));
    else
        pdu.appendCrc(crc16(pdu.data().data(), static_cast<uint16_t>(pdu.data().size())));
    return pdu;
}

std::optional<BufferStream> ModbusRTU::toPDU(FunctionCode expectFunctionCode, Buffer& adu) const
{
    if (adu.size() < minimumSize())
        return std::nullopt;
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
    return stream;
}

uint16_t ModbusRTU::aduMaximum() const { return 256; }

// RTU最小数据包 id + functionCode + crc16 = 4
uint16_t ModbusRTU::minimumSize() const { return 4; };

bool ModbusRTU::checkTail(BufferStream& stream) const
{
    if (stream.size() < 2)
        return false;
    if (crc16(stream.data().data(), static_cast<uint16_t>(stream.data().size() - sizeof(uint16_t))) !=
        (m_useBigendianCRC16 ? stream.msbCrc() : stream.lsbCrc()))
        throw std::exception("Response data crc error.");
    return true;
}

} // namespace ModbusOver