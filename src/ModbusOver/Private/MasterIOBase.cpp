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
#include "MasterIOBase.h"
#include "AbstractProtocol.h"
#include "Buffer.h"

namespace ModbusOver
{
std::vector<uint8_t> MasterIOBase::readCoils(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    write(m_protocol->requestReadColis(startingAddress, quantityOfCoils).data());
    std::vector<uint8_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onResponseReadColis(buffer, ret))
            return ret;
    }
}

std::vector<uint8_t> MasterIOBase::readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    write(m_protocol->requestReadDiscreteInputs(startingAddress, quantityOfCoils).data());
    std::vector<uint8_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onResponseReadDiscreteInputs(buffer, ret))
            return ret;
    }
}

std::vector<uint16_t> MasterIOBase::readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    write(m_protocol->requestReadHoldingRegisters(startingAddress, quantityOfRegisters).data());
    std::vector<uint16_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onResponseReadHoldingRegisters(buffer, ret))
            return ret;
    }
}

std::vector<uint16_t> MasterIOBase::readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    write(m_protocol->requestReadInputRegisters(startingAddress, quantityOfRegisters).data());
    std::vector<uint16_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onResponseReadInputRegisters(buffer, ret))
            return ret;
    }
}

void MasterIOBase::writeSingleCoil(uint16_t address, bool on)
{
    write(m_protocol->requestWriteSingleCoil(address, on).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onResponseWriteSingleCoil(buffer))
            return;
    }
}

void MasterIOBase::writeSingleRegister(uint16_t address, uint16_t value)
{
    write(m_protocol->requestWriteSingleRegister(address, value).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onResponseWriteSingleRegister(buffer))
            return;
    }
}

void MasterIOBase::writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, std::vector<uint8_t>&& states)
{
    write(m_protocol->requestWriteMultipleCoils(startingAddress, quantityOfCoils, std::move(states)).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onResponseWriteMultipleCoils(buffer))
            return;
    }
}

void MasterIOBase::writeMultipleRegisters(uint16_t startingAddress, std::vector<uint16_t>&& values)
{
    write(m_protocol->requestWriteMultipleRegisters(startingAddress, std::move(values)).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onResponseWriteMultipleRegisters(buffer))
            return;
    }
}

std::vector<uint8_t> MasterIOBase::reportServerID()
{
    std::vector<uint8_t> data;
    write(m_protocol->requestReportServerID().data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onRequestReportServerID(buffer, data))
            return data;
    }
}

uint8_t MasterIOBase::readExceptionStatus()
{
    uint8_t data;
    write(m_protocol->requestReadExceptionStatus().data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_protocol->onRequestReadExceptionStatus(buffer, data))
            return data;
    }
}

} // namespace ModbusOver