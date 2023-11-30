/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023 LiLong                                                   **
**  This file is part of ModbusOver.                                             **
**                                                                              **
**  ModbusOver is free software: you can redistribute it and/or modify           **
**  it under the terms of the GNU Lesser General Public License as published by **
**  the Free Software Foundation, either version 3 of the License, or           **
**  (at your option) any later version.                                         **
**                                                                              **
**  ModbusOver is distributed in the hope that it will be useful,                **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of              **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               **
**  GNU Lesser General Public License for more details.                         **
**                                                                              **
**  You should have received a copy of the GNU Lesser General Public License    **
**  along with ModbusOver.  If not, see <https://www.gnu.org/licenses/>.         **
**********************************************************************************/
#pragma once

#include "Address.h"
#include "modbuscpp_global.h"

#include <chrono>

namespace ModbusOver
{
class MODBUSOVER_EXPORT Master
{
public:
    Master();
    virtual ~Master();
    template<IsAddress T>
    void connect(const T& address, const std::chrono::milliseconds& connectTimeout);
    void close() noexcept;
    void setTimeout(const std::chrono::milliseconds& timeout);
    const std::chrono::milliseconds& timeout() const;
    void setSlave(uint8_t slave);
    uint8_t slave() const;
    std::vector<uint8_t> readCoils(uint16_t startingAddress, uint16_t quantityOfCoils);
    std::vector<uint8_t> readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils);
    std::vector<uint16_t> readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters);
    std::vector<uint16_t> readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters);
    void writeSingleCoil(uint16_t address, bool on);
    void writeSingleRegister(uint16_t address, uint16_t value);
    void writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, std::vector<uint8_t>&& states);
    void writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, const std::vector<uint8_t>& states);
    void writeMultipleRegisters(uint16_t startingAddress, std::vector<uint16_t>&& values);
    void writeMultipleRegisters(uint16_t startingAddress, const std::vector<uint16_t>& values);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace ModbusOver