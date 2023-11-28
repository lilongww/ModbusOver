#include "ModbusRTU.h"
#include "Buffer.h"
#include "CRC.h"

namespace ModbusCpp
{
Buffer ModbusRTU::toADU(Buffer pdu) const
{
    pdu.appendCrc(crc16(pdu.data().data(), pdu.data().size()));
    return pdu;
}

std::optional<Buffer> ModbusRTU::toPDU(Buffer adu) const
{
    adu.data().erase(adu.data().begin() + adu.data().size() - 2, adu.data().end());
    return adu;
}

uint16_t ModbusRTU::aduMaximum() const { return 256; }
} // namespace ModbusCpp