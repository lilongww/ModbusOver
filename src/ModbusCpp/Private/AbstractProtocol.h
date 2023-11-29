/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023 LiLong                                                   **
**  This file is part of ModbusCpp.                                             **
**                                                                              **
**  ModbusCpp is free software: you can redistribute it and/or modify           **
**  it under the terms of the GNU Lesser General Public License as published by **
**  the Free Software Foundation, either version 3 of the License, or           **
**  (at your option) any later version.                                         **
**                                                                              **
**  ModbusCpp is distributed in the hope that it will be useful,                **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of              **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               **
**  GNU Lesser General Public License for more details.                         **
**                                                                              **
**  You should have received a copy of the GNU Lesser General Public License    **
**  along with ModbusCpp.  If not, see <https://www.gnu.org/licenses/>.         **
**********************************************************************************/
#pragma once

#include "../Standard.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ModbusCpp
{
class Buffer;
class BufferStream;
class AbstractProtocol
{
public:
    enum class FunctionCode : uint8_t
    {
        ReadColis                      = 0x01,
        ReadDiscreteInputs             = 0x02,
        ReadHoldingRegisters           = 0x03,
        ReadInputRegisters             = 0x04,
        WriteSingleCoil                = 0x05,
        WriteSingleRegister            = 0x06,
        ReadExceptionStatus            = 0x07, // Serial Line only
        Diagnostics                    = 0x08, // Serial Line only
        GetCommEventCounter            = 0x0B, // Serial Line only
        GetCommEventLog                = 0x0C, // Serial Line only
        WriteMultipleCoils             = 0x0F,
        WriteMultipleRegisters         = 0x10,
        ReportServerID                 = 0x11, // Serial Line only
        ReadFileRecord                 = 0x14,
        WriteFileRecord                = 0x15,
        MaskWriteRegister              = 0x16,
        ReadWriteMultipleRegisters     = 0x17,
        ReadFIFOQueue                  = 0x18,
        EncapsulatedInterfaceTransport = 0x2B
    };
    inline AbstractProtocol(const uint8_t& slave) : m_slave(slave) {}
    virtual ~AbstractProtocol();
    Buffer requestReadColis(uint16_t startingAddress, uint16_t quantityOfCoils) const;
    bool onResponseReadColis(Buffer& buffer, std::vector<uint8_t>& status) const;
    Buffer requestReadDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils) const;
    bool onResponseReadDiscreteInputs(Buffer& buffer, std::vector<uint8_t>& status) const;
    static std::shared_ptr<AbstractProtocol> create(ModbusProtocol proto, const uint8_t& slave);

protected:
    virtual Buffer toADU(Buffer pdu) const                                                           = 0;
    virtual std::optional<BufferStream> toCommon(FunctionCode expectFunctionCode, Buffer& adu) const = 0;
    virtual uint16_t aduMaximum() const                                                              = 0;
    virtual uint16_t minimumSize() const                                                             = 0;
    constexpr static uint8_t toExceptionCode(FunctionCode code) { return static_cast<uint8_t>(code) + ExceptionCodeAddend; }
    constexpr static FunctionCode fromExceptionCode(uint8_t exceptionCode)
    {
        return static_cast<FunctionCode>(exceptionCode - ExceptionCodeAddend);
    }
    void checkException(FunctionCode code, uint8_t receiveCode, BufferStream& stream) const;

protected:
    const uint8_t& m_slave;
};
} // namespace ModbusCpp