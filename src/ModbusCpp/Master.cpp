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
#include "Master.h"
#include "Private/CommonData.h"
#include "Private/MasterSerialPort.h"

namespace ModbusCpp
{
struct Master::Impl
{
    std::shared_ptr<MasterIOBase> iobase;
    MasterCommonData common;
};

Master::Master() : m_impl(std::make_unique<Impl>()) {}

Master::~Master() {}

void Master::setTimeout(const std::chrono::milliseconds& timeout) { m_impl->common.timeout = timeout; }

const std::chrono::milliseconds& Master::timeout() const { return m_impl->common.timeout; }

void Master::setSlave(uint8_t slave) { m_impl->common.slave = slave; }

uint8_t Master::slave() const { return m_impl->common.slave; }

std::vector<uint8_t> Master::readCoils(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    return m_impl->iobase->readCoils(startingAddress, quantityOfCoils);
}

std::vector<uint8_t> Master::readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    return m_impl->iobase->readDiscreteInputs(startingAddress, quantityOfCoils);
}

std::vector<uint16_t> Master::readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    return m_impl->iobase->readHoldingRegisters(startingAddress, quantityOfRegisters);
}

template<>
MODBUSCPP_EXPORT void Master::connect<Address<AddressType::SerialPort>>(const Address<AddressType::SerialPort>& address,
                                                                        const std::chrono::milliseconds& connectTimeout)
{
    m_impl->iobase = std::make_shared<MasterSerialPort>(m_impl->common);
}

} // namespace ModbusCpp