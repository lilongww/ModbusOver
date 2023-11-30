﻿/*********************************************************************************
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

#include "../Address.h"
#include "MasterIOBase.h"

namespace ModbusCpp
{
class MasterSerialPort : public MasterIOBase
{
public:
    MasterSerialPort(const MasterCommonData& data);
    ~MasterSerialPort();
    void connect(const Address<AddressType::SerialPort>& address, const std::chrono::milliseconds& connectTimeout);
    std::vector<uint8_t> read() override;
    void write(std::vector<uint8_t>&& data) override;
    void close() noexcept override;
    bool connected() const noexcept override;
    void setBaudRate(uint32_t baudRate);
    void setDataBits(DataBits bits);
    void setFlowControl(FlowControl fc);
    void setParity(Parity p);
    void setStopBits(StopBits bits);
    std::vector<uint8_t> readCoils(uint16_t startingAddress, uint16_t quantityOfCoils) override;
    std::vector<uint8_t> readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils) override;
    std::vector<uint16_t> readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters) override;
    std::vector<uint16_t> readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters) override;
    void writeSingleCoil(uint16_t address, bool on) override;
    void writeSingleRegister(uint16_t address, uint16_t value) override;
    void writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, std::vector<uint8_t>&& states) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace ModbusCpp