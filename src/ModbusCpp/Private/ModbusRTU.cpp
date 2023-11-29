#include "ModbusRTU.h"
#include "Buffer.h"
#include "BufferStream.h"
#include "CRC.h"

namespace ModbusCpp
{
Buffer ModbusRTU::toADU(Buffer pdu) const
{
    pdu.appendCrc(crc16(pdu.data().data(), static_cast<uint16_t>(pdu.data().size())));
    return pdu;
}

std::optional<BufferStream> ModbusRTU::toCommon(FunctionCode expectFunctionCode, Buffer& adu) const
{
    if (adu.size() < minimumSize())
        return std::nullopt;
    BufferStream stream(adu);
    uint8_t slave, receiveCode;
    stream >> slave >> receiveCode;
    if (slave != m_slave) // 非请求从机回复
    {
        adu.data().clear();
        return std::nullopt;
    }
    checkException(expectFunctionCode, receiveCode, stream);
    if (crc16(adu.data().data(), static_cast<uint16_t>(adu.size() - sizeof(uint16_t))) != stream.crc())
        throw std::exception("Response data crc error.");
    return BufferStream(adu);
}

uint16_t ModbusRTU::aduMaximum() const { return 256; }

// RTU最小数据包 id + functionCode + crc16 = 4
uint16_t ModbusRTU::minimumSize() const { return 4; };
} // namespace ModbusCpp