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
#include "Master.h"
#include "Private/CommonData.h"
#include "Private/MasterSerialPort.h"
#include "Private/MasterTCP.h"

namespace ModbusOver
{
inline void throwUnconnected(const std::shared_ptr<MasterIOBase>& ptr)
{
    if (!ptr)
    {
        throw std::runtime_error("No connection.");
    }
}
struct Master::Impl
{
    std::shared_ptr<MasterIOBase> iobase;
    MasterCommonData common;
};

Master::Master() : m_impl(std::make_unique<Impl>()) {}

Master::~Master() {}

void Master::close() noexcept
{
    if (m_impl->iobase)
        m_impl->iobase->close();
}

void Master::setTimeout(const std::chrono::milliseconds& timeout) { m_impl->common.timeout = timeout; }

const std::chrono::milliseconds& Master::timeout() const { return m_impl->common.timeout; }

void Master::setSlave(uint8_t slave) { m_impl->common.slave = slave; }

uint8_t Master::slave() const { return m_impl->common.slave; }

std::vector<uint8_t> Master::readCoils(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    throwUnconnected(m_impl->iobase);
    return m_impl->iobase->readCoils(startingAddress, quantityOfCoils);
}

std::vector<uint8_t> Master::readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    throwUnconnected(m_impl->iobase);
    return m_impl->iobase->readDiscreteInputs(startingAddress, quantityOfCoils);
}

std::vector<uint16_t> Master::readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    throwUnconnected(m_impl->iobase);
    return m_impl->iobase->readHoldingRegisters(startingAddress, quantityOfRegisters);
}

std::vector<uint16_t> Master::readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    throwUnconnected(m_impl->iobase);
    return m_impl->iobase->readInputRegisters(startingAddress, quantityOfRegisters);
}

void Master::writeSingleCoil(uint16_t address, bool on)
{
    throwUnconnected(m_impl->iobase);
    m_impl->iobase->writeSingleCoil(address, on);
}

void Master::writeSingleRegister(uint16_t address, uint16_t value)
{
    throwUnconnected(m_impl->iobase);
    m_impl->iobase->writeSingleRegister(address, value);
}

void Master::writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, std::vector<uint8_t>&& states)
{
    throwUnconnected(m_impl->iobase);
    m_impl->iobase->writeMultipleCoils(startingAddress, quantityOfCoils, std::move(states));
}

void Master::writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, const std::vector<uint8_t>& states)
{
    writeMultipleCoils(startingAddress, quantityOfCoils, std::vector<uint8_t>(states.begin(), states.end()));
}

void Master::writeMultipleRegisters(uint16_t startingAddress, std::vector<uint16_t>&& values)
{
    throwUnconnected(m_impl->iobase);
    m_impl->iobase->writeMultipleRegisters(startingAddress, std::move(values));
}

void Master::writeMultipleRegisters(uint16_t startingAddress, const std::vector<uint16_t>& values)
{
    writeMultipleRegisters(startingAddress, std::vector<uint16_t>(values.begin(), values.end()));
}

template<>
MODBUSOVER_EXPORT void Master::connect<Address<AddressType::SerialPort>>(const Address<AddressType::SerialPort>& address,
                                                                        const std::chrono::milliseconds& connectTimeout)
{
    m_impl->iobase = std::make_shared<MasterSerialPort>(m_impl->common);
}

template<>
MODBUSOVER_EXPORT void Master::connect<Address<AddressType::TCP>>(const Address<AddressType::TCP>& address,
                                                                 const std::chrono::milliseconds& connectTimeout)
{
    m_impl->iobase = std::make_shared<MasterTCP>(m_impl->common);
}

} // namespace ModbusOver