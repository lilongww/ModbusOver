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
#include "MasterSerialPort.h"
#include "AbstractProtocol.h"

#include <boost/asio.hpp>

namespace ModbusOver
{
struct MasterSerialPort::Impl
{
    boost::asio::io_context io;
    boost::asio::serial_port serialPort { io };
    boost::asio::io_context::work worker { io };
    std::jthread thread;
    std::vector<uint8_t> writeBuffer;
    boost::asio::streambuf readBuffer;
    std::chrono::milliseconds rtsClock { std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()) };
};

MasterSerialPort::MasterSerialPort(const MasterCommonData& data) : MasterIOBase(data), m_impl(std::make_unique<Impl>())
{
    m_impl->thread = std::jthread(
        static_cast<boost::asio::io_context::count_type (boost::asio::io_context::*)()>(&boost::asio::io_context::run), &m_impl->io);
}

MasterSerialPort::~MasterSerialPort()
{
    if (m_impl->thread.joinable())
    {
        if (m_impl->serialPort.is_open())
            m_impl->serialPort.close();
        m_impl->io.stop();
    }
}

void MasterSerialPort::connect(const Address<AddressType::SerialPort>& address, const std::chrono::milliseconds& connectTimeout)
{
    m_impl->serialPort.open(address.portName());
    setBaudRate(address.baudRate());
    setDataBits(address.dataBits());
    setFlowControl(address.flowControl());
    setParity(address.parity());
    setStopBits(address.stopBits());
    m_protocol = AbstractProtocol::create(address.protocol(), m_data.slave, m_data.useBigendianCRC16, m_data.modbusAsciiLF);
}

std::vector<uint8_t> MasterSerialPort::read()
{
    auto mutex = std::make_shared<std::timed_mutex>();
    mutex->lock();
    auto error = std::make_shared<boost::system::error_code>();
    auto size  = std::make_shared<std::size_t>(0);
    auto ret   = std::make_shared<std::vector<uint8_t>>(m_protocol->aduMaximum());
    m_impl->io.post(
        [=]()
        {
            if (m_protocol->proto() == ModbusProtocol::ModbusASCII)
            {
                boost::asio::async_read_until(m_impl->serialPort,
                                              m_impl->readBuffer,
                                              std::string { '\x0D', m_data.modbusAsciiLF },
                                              [=](const boost::system::error_code& e, std::size_t s)
                                              {
                                                  *size  = s;
                                                  *error = e;
                                                  mutex->unlock();
                                              });
            }
            else
            {
                m_impl->serialPort.async_read_some(boost::asio::buffer(*ret),
                                                   [=](const boost::system::error_code& e, std::size_t s)
                                                   {
                                                       *size  = s;
                                                       *error = e;
                                                       mutex->unlock();
                                                   });
            }
        });

    if (!mutex->try_lock_for(m_data.timeout))
    {
        m_impl->serialPort.cancel();
        throw std::exception("Read timeout.");
    }
    if (*error != boost::system::errc::success)
    {
        m_impl->serialPort.close();
        boost::asio::detail::throw_error(*error, "readAllAscii");
    }
    if (m_protocol->proto() == ModbusProtocol::ModbusASCII)
    {
        auto begin = boost::asio::buffer_cast<const uint8_t*>(m_impl->readBuffer.data());
        *ret       = std::vector<uint8_t>(begin, begin + m_impl->readBuffer.size());
        m_impl->readBuffer.consume(m_impl->readBuffer.size());
    }
    else
    {
        ret->resize(*size);
    }
    return *ret;
}

void MasterSerialPort::write(std::vector<uint8_t>&& data)
{
    std::shared_ptr<void> scope = std::shared_ptr<void>(
        nullptr,
        [this](void*)
        { m_impl->rtsClock = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()); });
    for (;;)
    {
        if (std::chrono::system_clock::now().time_since_epoch() - m_impl->rtsClock < m_data.rtsDelay)
            std::this_thread::yield();
        else
            break;
    }
    m_impl->writeBuffer = data;
    auto mutex          = std::make_shared<std::timed_mutex>();
    mutex->lock();
    auto error = std::make_shared<boost::system::error_code>();
    m_impl->io.post(
        [=]()
        {
            m_impl->serialPort.async_write_some(boost::asio::buffer(m_impl->writeBuffer),
                                                [=](const boost::system::error_code& e, std::size_t)
                                                {
                                                    *error = e;
                                                    mutex->unlock();
                                                });
        });

    if (!mutex->try_lock_for(m_data.timeout))
    {
        m_impl->serialPort.cancel();
        throw std::exception("Send timeout.");
    }
    if (*error != boost::system::errc::success)
    {
        m_impl->serialPort.close();
        boost::asio::detail::throw_error(*error, "send");
    }
}

void MasterSerialPort::close() noexcept
{
    boost::system::error_code ec;
    m_impl->serialPort.close(ec);
}

bool MasterSerialPort::connected() const noexcept { return m_impl->serialPort.is_open(); }

void MasterSerialPort::setBaudRate(uint32_t baud) { m_impl->serialPort.set_option(boost::asio::serial_port::baud_rate { baud }); }

void MasterSerialPort::setDataBits(DataBits bits)
{
    m_impl->serialPort.set_option(boost::asio::serial_port::character_size { static_cast<unsigned int>(bits) });
}

void MasterSerialPort::setFlowControl(FlowControl fc)
{
    m_impl->serialPort.set_option(boost::asio::serial_port::flow_control { static_cast<boost::asio::serial_port::flow_control::type>(fc) });
}

void MasterSerialPort::setParity(Parity p)
{
    m_impl->serialPort.set_option(boost::asio::serial_port::parity { static_cast<boost::asio::serial_port::parity::type>(p) });
}

void MasterSerialPort::setStopBits(StopBits bits)
{
    m_impl->serialPort.set_option(boost::asio::serial_port::stop_bits { static_cast<boost::asio::serial_port::stop_bits::type>(bits) });
}

} // namespace ModbusOver