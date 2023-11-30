#include "AbstractProtocol.h"
#include "../ModbusException.h"
#include "BufferStream.h"
#include "Codec.h"
#include "ModbusRTU.h"
#include "ModbusTCP.h"

#include <cstring>
#include <functional>
#include <map>

namespace ModbusCpp
{

AbstractProtocol::~AbstractProtocol() {}

Buffer AbstractProtocol::requestReadColis(uint16_t startingAddress, uint16_t quantityOfCoils) const // quantityOfCoils 1-2000
{
    // if (quantityOfCoils < 1 || quantityOfCoils > 2000)
    //     throw std::runtime_error("Quantity of coils out of range: 1 - 2000.");
    return toADU(Codec<FunctionCode::ReadColis>::Request(m_slave, startingAddress, quantityOfCoils).encode());
}

bool AbstractProtocol::onResponseReadColis(Buffer& buffer, std::vector<uint8_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toCommon(FunctionCode::ReadColis, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadColis>::Response resp;
    if (!resp.decode(*stream))
        return false;
    status = std::move(resp).coilStatus();
    return true;
}

Buffer AbstractProtocol::requestReadDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils) const
{
    return toADU(Codec<FunctionCode::ReadDiscreteInputs>::Request(m_slave, startingAddress, quantityOfCoils).encode());
}

bool AbstractProtocol::onResponseReadDiscreteInputs(Buffer& buffer, std::vector<uint8_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toCommon(FunctionCode::ReadDiscreteInputs, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadDiscreteInputs>::Response resp;
    if (!resp.decode(*stream))
        return false;
    status = std::move(resp).coilStatus();
    return true;
}

Buffer AbstractProtocol::requestReadHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters) const
{
    return toADU(Codec<FunctionCode::ReadHoldingRegisters>::Request(m_slave, startingAddress, quantityOfRegisters).encode());
}

bool AbstractProtocol::onResponseReadHoldingRegisters(Buffer& buffer, std::vector<uint16_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toCommon(FunctionCode::ReadHoldingRegisters, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadHoldingRegisters>::Response resp;
    if (!resp.decode(*stream))
        return false;
    status.resize(resp.coilStatus().size() / 2);
    std::memcpy(status.data(), resp.coilStatus().data(), resp.coilStatus().size());
    std::ranges::transform(status, status.begin(), [](auto state) { return std::byteswap(state); });
    return true;
}

Buffer AbstractProtocol::requestReadInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters) const
{
    return toADU(Codec<FunctionCode::ReadInputRegisters>::Request(m_slave, startingAddress, quantityOfRegisters).encode());
}

bool AbstractProtocol::onResponseReadInputRegisters(Buffer& buffer, std::vector<uint16_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toCommon(FunctionCode::ReadInputRegisters, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadInputRegisters>::Response resp;
    if (!resp.decode(*stream))
        return false;
    status.resize(resp.coilStatus().size() / 2);
    std::memcpy(status.data(), resp.coilStatus().data(), resp.coilStatus().size());
    std::ranges::transform(status, status.begin(), [](auto state) { return std::byteswap(state); });
    return true;
}

Buffer AbstractProtocol::requestWriteSingleCoil(uint16_t address, bool state) const
{
    return toADU(Codec<FunctionCode::WriteSingleCoil>::Request(m_slave, address, state).encode());
}

bool AbstractProtocol::onResponseWriteSingleCoil(Buffer& buffer) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toCommon(FunctionCode::WriteSingleCoil, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::WriteSingleCoil>::Response resp;
    return resp.decode(*stream);
}

Buffer AbstractProtocol::requestWriteSingleRegister(uint16_t address, uint16_t value) const
{
    return toADU(Codec<FunctionCode::WriteSingleRegister>::Request(m_slave, address, value).encode());
}

bool AbstractProtocol::onResponseWriteSingleRegister(Buffer& buffer) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toCommon(FunctionCode::WriteSingleRegister, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::WriteSingleRegister>::Response resp;
    return resp.decode(*stream);
}

Buffer AbstractProtocol::requestWriteMultipleCoils(uint16_t startingAddress,
                                                   uint16_t quantityOfOutputs,
                                                   std::vector<uint8_t>&& states) const
{
    return toADU(Codec<FunctionCode::WriteMultipleCoils>::Request(m_slave, startingAddress, quantityOfOutputs, std::move(states)).encode());
}

bool AbstractProtocol::onResponseWriteMultipleCoils(Buffer& buffer) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toCommon(FunctionCode::WriteMultipleCoils, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::WriteMultipleCoils>::Response resp;
    return resp.decode(*stream);
}

std::shared_ptr<AbstractProtocol> AbstractProtocol::create(ModbusProtocol proto, const uint8_t& slave)
{
    switch (proto)
    {
    case ModbusProtocol::ModbusRTU:
        return std::make_shared<ModbusRTU>(slave);
    default:
        return nullptr;
    }
}

void AbstractProtocol::checkException(FunctionCode code, uint8_t receiveCode, BufferStream& stream) const
{
    if (toExceptionCode(code) == receiveCode)
    {
        ExceptionCode exceptionCode;
        stream >> exceptionCode;
        throw ModbusException(exceptionCode);
    }
}

} // namespace ModbusCpp