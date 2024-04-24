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
#include "AbstractProtocol.h"
#include "../ModbusException.h"
#include "BufferStream.h"
#include "Codec.h"
#include "ModbusASCII.h"
#include "ModbusRTU.h"
#include "ModbusTCP.h"

#include <cstring>
#include <functional>
#include <map>

namespace ModbusOver
{

AbstractProtocol::~AbstractProtocol() {}

Buffer AbstractProtocol::requestReadColis(uint16_t startingAddress, uint16_t quantityOfCoils) const // quantityOfCoils 1-2000
{
    // if (quantityOfCoils < 1 || quantityOfCoils > 2000)
    //     throw std::runtime_error("Quantity of coils out of range: 1 - 2000.");
    return toADU(Codec<FunctionCode::ReadColis>::Request(startingAddress, quantityOfCoils).encode());
}

bool AbstractProtocol::onResponseReadColis(Buffer& buffer, std::vector<uint8_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::ReadColis, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadColis>::Response resp;
    if (!resp.decode(*stream))
        return false;
    status = std::move(resp).coilStatus();
    checkTail(*stream);
    return true;
}

Buffer AbstractProtocol::requestReadDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils) const
{
    return toADU(Codec<FunctionCode::ReadDiscreteInputs>::Request(startingAddress, quantityOfCoils).encode());
}

bool AbstractProtocol::onResponseReadDiscreteInputs(Buffer& buffer, std::vector<uint8_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::ReadDiscreteInputs, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadDiscreteInputs>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    status = std::move(resp).coilStatus();
    return true;
}

Buffer AbstractProtocol::requestReadHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters) const
{
    return toADU(Codec<FunctionCode::ReadHoldingRegisters>::Request(startingAddress, quantityOfRegisters).encode());
}

bool AbstractProtocol::onResponseReadHoldingRegisters(Buffer& buffer, std::vector<uint16_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::ReadHoldingRegisters, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadHoldingRegisters>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    status.resize(resp.coilStatus().size() / 2);
    std::memcpy(status.data(), resp.coilStatus().data(), resp.coilStatus().size());
    std::ranges::transform(status, status.begin(), [](auto state) { return std::byteswap(state); });
    return true;
}

Buffer AbstractProtocol::requestReadInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters) const
{
    return toADU(Codec<FunctionCode::ReadInputRegisters>::Request(startingAddress, quantityOfRegisters).encode());
}

bool AbstractProtocol::onResponseReadInputRegisters(Buffer& buffer, std::vector<uint16_t>& status) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::ReadInputRegisters, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadInputRegisters>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    status.resize(resp.coilStatus().size() / 2);
    std::memcpy(status.data(), resp.coilStatus().data(), resp.coilStatus().size());
    std::ranges::transform(status, status.begin(), [](auto state) { return std::byteswap(state); });
    return true;
}

Buffer AbstractProtocol::requestWriteSingleCoil(uint16_t address, bool state) const
{
    return toADU(Codec<FunctionCode::WriteSingleCoil>::Request(address, state).encode());
}

bool AbstractProtocol::onResponseWriteSingleCoil(Buffer& buffer) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::WriteSingleCoil, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::WriteSingleCoil>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    return true;
}

Buffer AbstractProtocol::requestWriteSingleRegister(uint16_t address, uint16_t value) const
{
    return toADU(Codec<FunctionCode::WriteSingleRegister>::Request(address, value).encode());
}

bool AbstractProtocol::onResponseWriteSingleRegister(Buffer& buffer) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::WriteSingleRegister, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::WriteSingleRegister>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    return true;
}

Buffer AbstractProtocol::requestWriteMultipleCoils(uint16_t startingAddress,
                                                   uint16_t quantityOfOutputs,
                                                   std::vector<uint8_t>&& states) const
{
    return toADU(Codec<FunctionCode::WriteMultipleCoils>::Request(startingAddress, quantityOfOutputs, std::move(states)).encode());
}

bool AbstractProtocol::onResponseWriteMultipleCoils(Buffer& buffer) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::WriteMultipleCoils, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::WriteMultipleCoils>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    return true;
}

Buffer AbstractProtocol::requestWriteMultipleRegisters(uint16_t startingAddress, std::vector<uint16_t>&& values) const
{
    return toADU(
        Codec<FunctionCode::WriteMultipleRegisters>::Request(startingAddress, static_cast<uint16_t>(values.size()), std::move(values))
            .encode());
}

bool AbstractProtocol::onResponseWriteMultipleRegisters(Buffer& buffer) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::WriteMultipleRegisters, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::WriteMultipleRegisters>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    return true;
}

Buffer AbstractProtocol::requestReportServerID() const { return toADU(Codec<FunctionCode::ReportServerID>::Request().encode()); }

bool AbstractProtocol::onRequestReportServerID(Buffer& buffer, std::vector<uint8_t>& data) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::ReportServerID, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReportServerID>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    data = std::move(resp);
    return true;
}

Buffer AbstractProtocol::requestReadExceptionStatus() const { return toADU(Codec<FunctionCode::ReadExceptionStatus>::Request().encode()); }

bool AbstractProtocol::onRequestReadExceptionStatus(Buffer& buffer, uint8_t& data) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::ReadExceptionStatus, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadExceptionStatus>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    data = resp.outputData();
    return true;
}

Buffer AbstractProtocol::requestGetCommEventCounter() const { return toADU(Codec<FunctionCode::GetCommEventCounter>::Request().encode()); }

bool AbstractProtocol::onRequestGetCommEventCounter(Buffer& buffer, uint16_t& status, uint16_t& eventCount) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::GetCommEventCounter, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::GetCommEventCounter>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    status     = resp.status();
    eventCount = resp.eventCount();
    return true;
}

Buffer AbstractProtocol::requestGetCommEventLog() const { return toADU(Codec<FunctionCode::GetCommEventLog>::Request().encode()); }

bool AbstractProtocol::onRequestGetCommEventLog(Buffer& buffer, CommEventLog& log) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::GetCommEventLog, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::GetCommEventLog>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    log = std::move(resp);
    return true;
}

Buffer AbstractProtocol::requestMaskWriteRegister(uint16_t address, uint16_t andMask, uint16_t orMask) const
{
    return toADU(Codec<FunctionCode::MaskWriteRegister>::Request(address, andMask, orMask).encode());
}

bool AbstractProtocol::onRequestMaskWriteRegister(Buffer& buffer, uint16_t& address, uint16_t& andMask, uint16_t& orMask) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::MaskWriteRegister, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::MaskWriteRegister>::Response<FunctionCode::MaskWriteRegister> resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    address = resp.address();
    andMask = resp.andMask();
    orMask  = resp.orMask();
    return true;
}

Buffer AbstractProtocol::requestReadWriteMultipleRegisters(uint16_t readStartAddress,
                                                           uint16_t quantityToRead,
                                                           uint16_t writeStartAddress,
                                                           std::vector<uint16_t>&& writeData) const
{
    return toADU(
        Codec<FunctionCode::ReadWriteMultipleRegisters>::Request(readStartAddress, quantityToRead, writeStartAddress, std::move(writeData))
            .encode());
}

bool AbstractProtocol::onRequestReadWriteMultipleRegisters(Buffer& buffer, std::vector<uint16_t>& data) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::ReadWriteMultipleRegisters, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadWriteMultipleRegisters>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    data = std::move(resp);
    return true;
}

Buffer AbstractProtocol::requestReadFIFOQueue(uint16_t address) const
{
    return toADU(Codec<FunctionCode::ReadFIFOQueue>::Request(address).encode());
}

bool AbstractProtocol::onRequestReadFIFOQueue(Buffer& buffer, std::vector<uint16_t>& data) const
{
    if (buffer.size() < minimumSize())
        return false;
    auto stream = toPDU(FunctionCode::ReadFIFOQueue, buffer);
    if (!stream)
        return false;
    Codec<FunctionCode::ReadFIFOQueue>::Response resp;
    if (!resp.decode(*stream))
        return false;
    checkTail(*stream);
    data = std::move(resp);
    return true;
}

std::shared_ptr<AbstractProtocol> AbstractProtocol::create(ModbusProtocol proto,
                                                           const uint8_t& slave,
                                                           const bool& useBigendianCRC16,
                                                           const char& asciiLF)
{
    switch (proto)
    {
    case ModbusProtocol::ModbusRTU:
        return std::make_shared<ModbusRTU>(slave, useBigendianCRC16);
    case ModbusProtocol::ModbusTCP:
        return std::make_shared<ModbusTCP>(slave);
    case ModbusProtocol::ModbusASCII:
        return std::make_shared<ModbusASCII>(slave, asciiLF);
    default:
        return nullptr;
    }
}

void AbstractProtocol::checkException(FunctionCode code, uint8_t receiveCode, BufferStream& stream) const
{
    if (toExceptionCode(code) == receiveCode)
    {
        ExceptionCode exceptionCode;
        stream >> receiveCode >> exceptionCode;
        throw ModbusException(exceptionCode);
    }
}

} // namespace ModbusOver