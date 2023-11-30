/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023 LiLong                                                   **
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

#include "../Standard.h"
#include "CommonData.h"

#include <chrono>
#include <string>

namespace ModbusOver
{
class AbstractProtocol;
class MasterIOBase
{
public:
    MasterIOBase(const MasterCommonData& data) : m_data(data) {}
    virtual ~MasterIOBase() noexcept                = default;
    virtual std::vector<uint8_t> read()             = 0;
    virtual void write(std::vector<uint8_t>&& data) = 0;
    virtual void close() noexcept                   = 0;
    virtual bool connected() const noexcept         = 0;
    std::vector<uint8_t> readCoils(uint16_t startingAddress, uint16_t quantityOfCoils);
    std::vector<uint8_t> readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils);
    std::vector<uint16_t> readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters);
    std::vector<uint16_t> readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters);
    void writeSingleCoil(uint16_t address, bool on);
    void writeSingleRegister(uint16_t address, uint16_t value);
    void writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, std::vector<uint8_t>&& states);
    void writeMultipleRegisters(uint16_t startingAddress, std::vector<uint16_t>&& values);

protected:
    bool isBroadcast() const { return m_data.slave == Broadcast; }

protected:
    const MasterCommonData& m_data;
    std::shared_ptr<AbstractProtocol> m_protocol;
};
} // namespace ModbusOver