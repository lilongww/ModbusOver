#include "AbstractProtocol.h"
#include "BufferStream.h"

#include <functional>
#include <map>

namespace ModbusCpp
{

AbstractProtocol::~AbstractProtocol() {}

Buffer AbstractProtocol::requestReadColis(uint16_t startingAddress, uint16_t quantityOfCoils) const
{
    return toADU(Buffer { m_slave, startingAddress, quantityOfCoils });
}

bool AbstractProtocol::onResponseReadColis(Buffer& buffer, uint8_t slave, std::vector<uint8_t>& status) const
{
    if (buffer.size() < 3)
        return false;
}

} // namespace ModbusCpp