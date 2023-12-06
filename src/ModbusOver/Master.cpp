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

/*!
    \class      ModbusOver::Master
    \brief      Modbus 客户端（主站）接口.
    \ingroup    ModbusOver
    \inmodule   ModbusOver
    \inheaderfile Master.h
*/

// clang-format off
/*!
    \fn         template<ModbusOver::IsAddress T> void ModbusOver::Master::connect(const T& address, const std::chrono::milliseconds& connectTimeout = std::chrono::milliseconds(5000));
    \brief      连接到地址为 \a address 的设备，连接超时时间为 \a connectTimeout.
    \sa         close
*/
// clang-format on

/*!
    \brief      构造函数.
*/
Master::Master() : m_impl(std::make_unique<Impl>()) {}

/*!
    \brief      析构函数.
*/
Master::~Master() {}

/*!
    \brief      关闭连接.
    \sa         connect
*/
void Master::close() noexcept
{
    if (m_impl->iobase)
        m_impl->iobase->close();
}

/*!
    \brief      设置超时时间 \a timeout.
    \sa         timeout
*/
void Master::setTimeout(const std::chrono::milliseconds& timeout) { m_impl->common.timeout = timeout; }

/*!
    \brief      返回超时时间.
    \sa         setTimeout
*/
const std::chrono::milliseconds& Master::timeout() const { return m_impl->common.timeout; }

/*!
    \brief      设置从机编号 \a slave.
    \sa         slave
*/
void Master::setSlave(uint8_t slave) { m_impl->common.slave = slave; }

/*!
    \brief      返回从机编号.
    \sa         setSlave
*/
uint8_t Master::slave() const { return m_impl->common.slave; }

/*!
    \brief      开启此选项 \a on, 将在ModbusRTU中使用MSB在前，LSB在后的CRC16校验码，以应对某些非标准仪器.
    \note       在标准ModbusRTU中，CRC16的校验码发送顺序是LSB在前，MSB在后.
    \sa         useBigendianCRC16
*/
void Master::setUseBigendianCRC16(bool on) { m_impl->common.useBigendianCRC16 = on; }

/*!
    \brief      返回在ModbusRTU中是否使用MSB在前，LSB在后的CRC16校验码.
    \sa         setUseBigendianCRC16
*/
bool Master::useBigendianCRC16() const { return m_impl->common.useBigendianCRC16; }

/*!
    \brief      设置RTS \a delay.
    \sa         rtsDelay
*/
void Master::setRTSDelay(const std::chrono::milliseconds& delay) { m_impl->common.rtsDelay = delay; }

/*!
    \brief      设置ModbusASCII协议自定义LF \a lf.
    \sa         asciiLF
*/
void Master::setAsciiLF(char lf) { m_impl->common.modbusAsciiLF = lf; }

/*!
    \brief      返回ModbusASCII协议自定义LF
    \sa         setAsciiLF
*/
char Master::asciiLF() const { return m_impl->common.modbusAsciiLF; }

/*!
    \brief      返回RTS delay.
    \sa         setRTSDelay
*/
const std::chrono::milliseconds& Master::rtsDelay() const { return m_impl->common.rtsDelay; }

/*!
    \brief      功能码 0x01, 读取从地址 \a startingAddress 开始的 \a quantityOfCoils 个线圈.
    \note       quantityOfCoils表示线圈数量，每个线圈为 1 bit, 所以返回的字节数为 quantityOfCoils / 8 + ((quantityOfCoils % 8) ? 1 : 0).
    \sa         writeSingleCoil, writeMultipleCoils
*/
std::vector<uint8_t> Master::readCoils(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    throwUnconnected(m_impl->iobase);
    return m_impl->iobase->readCoils(startingAddress, quantityOfCoils);
}

/*!
    \brief      功能码 0x02, 读取从地址 \a startingAddress 开始的 \a quantityOfCoils 个离散量输入（线圈），离散量输入为只读.
*/
std::vector<uint8_t> Master::readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    throwUnconnected(m_impl->iobase);
    return m_impl->iobase->readDiscreteInputs(startingAddress, quantityOfCoils);
}

/*!
    \brief      功能码 0x03, 读取从地址 \a startingAddress 开始的 \a quantityOfRegisters 个保持寄存器.
    \sa         writeSingleRegister, writeMultipleRegisters
*/
std::vector<uint16_t> Master::readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    throwUnconnected(m_impl->iobase);
    return m_impl->iobase->readHoldingRegisters(startingAddress, quantityOfRegisters);
}

/*!
    \brief      功能码 0x04, 读取从地址 \a startingAddress 开始的 \a quantityOfRegisters 个输入寄存器，输入寄存器为只读.
*/
std::vector<uint16_t> Master::readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    throwUnconnected(m_impl->iobase);
    return m_impl->iobase->readInputRegisters(startingAddress, quantityOfRegisters);
}

/*!
    \brief      功能码 0x05, 写位于地址 \a address 的单个线圈状态 \a on.
    \sa         writeMultipleCoils, readCoils
*/
void Master::writeSingleCoil(uint16_t address, bool on)
{
    throwUnconnected(m_impl->iobase);
    m_impl->iobase->writeSingleCoil(address, on);
}

/*!
    \brief      功能码 0x06, 写位于地址 \a address 的单个保持寄存器值 \a value.
    \sa         writeMultipleRegisters, readHoldingRegisters
*/
void Master::writeSingleRegister(uint16_t address, uint16_t value)
{
    throwUnconnected(m_impl->iobase);
    m_impl->iobase->writeSingleRegister(address, value);
}

/*!
    \brief      功能码 0x0F, 写入从地址 \a startingAddress 开始的 \a quantityOfCoils 线圈状态 \a states.
    \note       quantityOfCoils表示线圈数量，每个线圈为 1 bit, 所以 \a states 的字节数为 quantityOfCoils / 8 + ((quantityOfCoils % 8) ? 1 :
   0).
    \sa         writeSingleCoil, readCoils
*/
void Master::writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, std::vector<uint8_t>&& states)
{
    throwUnconnected(m_impl->iobase);
    m_impl->iobase->writeMultipleCoils(startingAddress, quantityOfCoils, std::move(states));
}

/*!
    \overload   重载函数。
                功能码 0x0F, 写入从地址 \a startingAddress 开始的 \a quantityOfCoils 线圈状态 \a states.
    \note       quantityOfCoils表示线圈数量，每个线圈为 1 bit, 所以 \a states 的字节数为 quantityOfCoils / 8 + ((quantityOfCoils % 8) ? 1 :
   0).
    \sa         writeMultipleCoils
*/
void Master::writeMultipleCoils(uint16_t startingAddress, uint16_t quantityOfCoils, const std::vector<uint8_t>& states)
{
    writeMultipleCoils(startingAddress, quantityOfCoils, std::vector<uint8_t>(states.begin(), states.end()));
}

/*!
    \brief      功能码 0x10, 写入从地址 \a startingAddress 开始的 \a values 个数的保持寄存器状态.
    \sa         writeSingleRegister, readHoldingRegisters
*/
void Master::writeMultipleRegisters(uint16_t startingAddress, std::vector<uint16_t>&& values)
{
    throwUnconnected(m_impl->iobase);
    m_impl->iobase->writeMultipleRegisters(startingAddress, std::move(values));
}

/*!
    \overload   重载函数.
                功能码 0x10, 写入从地址 \a startingAddress 开始的 \a values 个数的保持寄存器状态.
    \sa         writeMultipleRegisters
*/
void Master::writeMultipleRegisters(uint16_t startingAddress, const std::vector<uint16_t>& values)
{
    writeMultipleRegisters(startingAddress, std::vector<uint16_t>(values.begin(), values.end()));
}

template<>
MODBUSOVER_EXPORT void Master::connect<Address<AddressType::SerialPort>>(const Address<AddressType::SerialPort>& address,
                                                                         const std::chrono::milliseconds& connectTimeout)
{
    auto io = std::make_shared<MasterSerialPort>(m_impl->common);
    io->connect(address, connectTimeout);
    m_impl->iobase = io;
}

template<>
MODBUSOVER_EXPORT void Master::connect<Address<AddressType::TCP>>(const Address<AddressType::TCP>& address,
                                                                  const std::chrono::milliseconds& connectTimeout)
{
    auto io = std::make_shared<MasterTCP>(m_impl->common);
    io->connect(address, connectTimeout);
    m_impl->iobase = io;
}

} // namespace ModbusOver