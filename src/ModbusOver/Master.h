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
#pragma once

#include "Address.h"
#include "modbusover_global.h"

#include <chrono>

namespace ModbusOver
{
class ProtocolDebug;
class MODBUSOVER_EXPORT Master
{
public:
    Master();
    virtual ~Master();
    template<IsAddress T>
    void connect(const T& address, const std::chrono::milliseconds& connectTimeout = std::chrono::milliseconds(5000));
    void close() noexcept;
    void setTimeout(const std::chrono::milliseconds& timeout);
    const std::chrono::milliseconds& timeout() const;
    void setSlave(uint8_t slave);
    uint8_t slave() const;
    void setUseBigendianCRC16(bool on);
    bool useBigendianCRC16() const;
    void setRTSDelay(const std::chrono::milliseconds& delay);
    void setAsciiLF(char lf);
    char asciiLF() const;
    const std::chrono::milliseconds& rtsDelay() const;
    std::vector<uint8_t> readCoils(uint16_t startingAddress, uint16_t quantityOfCoils);
    std::vector<uint8_t> readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils);
    std::vector<uint16_t> readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters);
    std::vector<uint16_t> readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters);
    void writeSingleCoil(uint16_t address, bool on);
    void writeSingleRegister(uint16_t address, uint16_t value);
    uint8_t readExceptionStatus();
    CommEventCounter getCommEventCounter();
    void writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, std::vector<uint8_t>&& states);
    void writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, const std::vector<uint8_t>& states);
    void writeMultipleRegisters(uint16_t startingAddress, std::vector<uint16_t>&& values);
    void writeMultipleRegisters(uint16_t startingAddress, const std::vector<uint16_t>& values);
    std::vector<uint8_t> reportServerID();
    void maskWriteRegister(uint16_t address, uint16_t andMask, uint16_t orMask);
    std::vector<uint16_t> readWriteMultipleRegisters(uint16_t readStartAddress,
                                                     uint16_t quantityToRead,
                                                     uint16_t writeStartAddress,
                                                     std::vector<uint16_t>&& writeData);
    std::vector<uint16_t> readWriteMultipleRegisters(uint16_t readStartAddress,
                                                     uint16_t quantityToRead,
                                                     uint16_t writeStartAddress,
                                                     const std::vector<uint16_t>& writeData);
    std::vector<uint16_t> requestReadFIFOQueue(uint16_t address);
    void setProtocolDebug(std::shared_ptr<ProtocolDebug> debug);
    std::shared_ptr<ProtocolDebug> protocolDebug() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace ModbusOver