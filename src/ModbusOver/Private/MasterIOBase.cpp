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
#include "../ProtocolDebug.h"
#include "AbstractProtocol.h"
#include "Buffer.h"

namespace ModbusOver
{

std::vector<uint8_t> MasterIOBase::readData()
{
    if (m_data.protoDebug)
    {
        auto data = read();
        m_debugData.append_range(data);
        return data;
    }
    else
        return read();
}

void MasterIOBase::writeData(std::vector<uint8_t>&& data)
{
    if (m_data.protoDebug)
    {
        m_debugData.append_range(data);
        write(std::move(data));
        finishDebugTx();
    }
    else
        write(std::move(data));
}

std::vector<uint8_t> MasterIOBase::readCoils(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    writeData(m_protocol->requestReadColis(startingAddress, quantityOfCoils).data());
    std::vector<uint8_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onResponseReadColis(buffer, ret))
        {
            finishDebugRx();
            return ret;
        }
    }
}

std::vector<uint8_t> MasterIOBase::readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    writeData(m_protocol->requestReadDiscreteInputs(startingAddress, quantityOfCoils).data());
    std::vector<uint8_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onResponseReadDiscreteInputs(buffer, ret))
        {
            finishDebugRx();
            return ret;
        }
    }
}

std::vector<uint16_t> MasterIOBase::readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    writeData(m_protocol->requestReadHoldingRegisters(startingAddress, quantityOfRegisters).data());
    std::vector<uint16_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onResponseReadHoldingRegisters(buffer, ret))
        {
            finishDebugRx();
            return ret;
        }
    }
}

std::vector<uint16_t> MasterIOBase::readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    writeData(m_protocol->requestReadInputRegisters(startingAddress, quantityOfRegisters).data());
    std::vector<uint16_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onResponseReadInputRegisters(buffer, ret))
        {
            finishDebugRx();
            return ret;
        }
    }
}

void MasterIOBase::writeSingleCoil(uint16_t address, bool on)
{
    writeData(m_protocol->requestWriteSingleCoil(address, on).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onResponseWriteSingleCoil(buffer))
        {
            finishDebugRx();
            return;
        }
    }
}

void MasterIOBase::writeSingleRegister(uint16_t address, uint16_t value)
{
    writeData(m_protocol->requestWriteSingleRegister(address, value).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onResponseWriteSingleRegister(buffer))
        {
            finishDebugRx();
            return;
        }
    }
}

void MasterIOBase::writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, std::vector<uint8_t>&& states)
{
    writeData(m_protocol->requestWriteMultipleCoils(startingAddress, quantityOfCoils, std::move(states)).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onResponseWriteMultipleCoils(buffer))
        {
            finishDebugRx();
            return;
        }
    }
}

void MasterIOBase::writeMultipleRegisters(uint16_t startingAddress, std::vector<uint16_t>&& values)
{
    writeData(m_protocol->requestWriteMultipleRegisters(startingAddress, std::move(values)).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onResponseWriteMultipleRegisters(buffer))
        {
            finishDebugRx();
            return;
        }
    }
}

std::vector<uint8_t> MasterIOBase::reportServerID()
{
    std::vector<uint8_t> data;
    writeData(m_protocol->requestReportServerID().data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onRequestReportServerID(buffer, data))
        {
            finishDebugRx();
            return data;
        }
    }
}

uint8_t MasterIOBase::readExceptionStatus()
{
    uint8_t data;
    writeData(m_protocol->requestReadExceptionStatus().data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onRequestReadExceptionStatus(buffer, data))
        {
            finishDebugRx();
            return data;
        }
    }
}

CommEventCounter MasterIOBase::getCommEventCounter()
{
    CommEventCounter cec;
    writeData(m_protocol->requestGetCommEventCounter().data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onRequestGetCommEventCounter(buffer, cec.status, cec.eventCount))
        {
            finishDebugRx();
            return cec;
        }
    }
}

CommEventLog MasterIOBase::getCommEventLog()
{
    CommEventLog log;
    writeData(m_protocol->requestGetCommEventLog().data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onRequestGetCommEventLog(buffer, log))
        {
            finishDebugRx();
            return log;
        }
    }
}

void MasterIOBase::maskWriteRegister(uint16_t address, uint16_t andMask, uint16_t orMask)
{
    uint16_t respAddress, respAndMask, respOrMask;
    writeData(m_protocol->requestMaskWriteRegister(address, andMask, orMask).data());
    if (isBroadcast())
        return;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onRequestMaskWriteRegister(buffer, respAddress, respAndMask, respOrMask) && respAddress == address &&
            respAndMask == andMask && respOrMask == orMask)
        {
            finishDebugRx();
            return;
        }
    }
}

std::vector<uint16_t> MasterIOBase::readWriteMultipleRegisters(uint16_t readStartAddress,
                                                               uint16_t quantityToRead,
                                                               uint16_t writeStartAddress,
                                                               std::vector<uint16_t>&& wdata)
{
    std::vector<uint16_t> data;
    writeData(m_protocol->requestReadWriteMultipleRegisters(readStartAddress, quantityToRead, writeStartAddress, std::move(wdata)).data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onRequestReadWriteMultipleRegisters(buffer, data))
        {
            finishDebugRx();
            return data;
        }
    }
}

std::vector<uint16_t> MasterIOBase::requestReadFIFOQueue(uint16_t address)
{
    std::vector<uint16_t> data;
    writeData(m_protocol->requestReadFIFOQueue(address).data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(readData());
        if (m_protocol->onRequestReadFIFOQueue(buffer, data))
        {
            finishDebugRx();
            return data;
        }
    }
}

void MasterIOBase::finishDebugRx()
{
    if (m_data.protoDebug)
    {
        m_data.protoDebug->rx(m_debugData);
        m_debugData.clear();
    }
}

void MasterIOBase::finishDebugTx()
{
    if (m_data.protoDebug)
    {
        m_data.protoDebug->tx(m_debugData);
        m_debugData.clear();
    }
}

} // namespace ModbusOver