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

std::optional<BufferStream> ModbusRTU::toPDU(Buffer& adu) const
{
    adu.data().erase(adu.data().begin() + adu.data().size() - 2, adu.data().end());
    return BufferStream(adu);
}

uint16_t ModbusRTU::aduMaximum() const { return 256; }

// RTU最小数据包 id + functionCode + crc16 = 4
uint16_t ModbusRTU::minimumSize() const { return 4; };

} // namespace ModbusCpp