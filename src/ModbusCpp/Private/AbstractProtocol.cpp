#include "AbstractProtocol.h"
#include "../ModbusException.h"
#include "BufferStream.h"

#include <functional>
#include <map>

namespace ModbusCpp
{

AbstractProtocol::~AbstractProtocol() {}

Buffer AbstractProtocol::requestReadColis(uint16_t startingAddress, uint16_t quantityOfCoils) const // quantityOfCoils 1-2000
{
    if (quantityOfCoils < 1 || quantityOfCoils > 2000)
        throw std::runtime_error("Quantity of coils out of range: 1 - 2000.");
    return toADU(Buffer { m_slave, startingAddress, quantityOfCoils });
}

bool AbstractProtocol::onResponseReadColis(Buffer& buffer, uint8_t& slave, std::vector<uint8_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    peakSlave(slave);
    auto stream = toPDU(buffer);
    if (!stream)
        return false;
    if (stream->size() < 2)
        return false;
    uint8_t size;
    *stream >> size;
    if (stream->size() < size)
        return false;
    status.resize(size);
    std::ranges::for_each(status, [&](auto& state) { *stream >> state; });
    return true;
}

void AbstractProtocol::checkException(uint8_t slave, BufferStream& stream) const
{
    if (slave == toExceptionCode(m_slave))
    {
        ExceptionCode exceptionCode;
        stream >> exceptionCode;
        throw ModbusException(exceptionCode);
    }
}

} // namespace ModbusCpp