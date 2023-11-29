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

void Master::setTimeout(const std::chrono::milliseconds& timeout)
{
    if (m_impl->iobase)
    {
        m_impl->iobase->setTimeout(timeout);
    }
}

const std::chrono::milliseconds& Master::timeout() const { return m_impl->common.timeout; }

void Master::setSlave(uint8_t slave)
{
    if (m_impl->iobase)
    {
        m_impl->iobase->setSlave(slave);
    }
}

uint8_t Master::slave() const { return m_impl->common.slave; }

template<>
MODBUSCPP_EXPORT void Master::connect<Address<AddressType::SerialPort>>(const Address<AddressType::SerialPort>& address,
                                                                        const std::chrono::milliseconds& connectTimeout)
{
    m_impl->iobase = std::make_shared<MasterSerialPort>(m_impl->common);
}

} // namespace ModbusCpp