/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023-2026 LiLong                                              **
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
#include "MasterUDP.h"
#include "../ProtocolDebug.h"
#include "AbstractProtocol.h"

#include <boost/asio.hpp>

namespace ModbusOver
{
struct MasterUDP::Impl
{
    boost::asio::io_context io;
    boost::asio::ip::udp::socket socket { io };
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work { boost::asio::make_work_guard(io) };
    std::jthread thread;
    std::vector<uint8_t> writeBuffer;
};

MasterUDP::MasterUDP(const MasterCommonData& data) : MasterIOBase(data), m_impl(std::make_unique<Impl>())
{
    m_impl->thread = std::jthread(
        static_cast<boost::asio::io_context::count_type (boost::asio::io_context::*)()>(&boost::asio::io_context::run), &m_impl->io);
}

MasterUDP::~MasterUDP()
{
    if (m_impl->thread.joinable())
    {
        if (m_impl->socket.is_open())
            m_impl->socket.close();
        m_impl->io.stop();
    }
}

void MasterUDP::connect(const Address<AddressType::UDP>& address, const std::chrono::milliseconds& connectTimeout)
{
    auto error = std::make_shared<boost::system::error_code>();
    auto mutex = std::make_shared<std::timed_mutex>();
    mutex->lock();
    m_impl->socket.async_connect(boost::asio::ip::udp::endpoint(boost::asio::ip::make_address(address.ip()), address.port()),
                                 [=](const boost::system::error_code& e)
                                 {
                                     *error = e;
                                     mutex->unlock();
                                 });

    if (!mutex->try_lock_for(connectTimeout))
    {
        m_impl->socket.close();
        throw std::exception("Connect timeout.");
    }
    if (*error != boost::system::errc::success)
    {
        boost::asio::detail::throw_error(*error, "connect");
    }
    m_protocol = AbstractProtocol::create(address.protocol(), m_data.slave, m_data.useBigendianCRC16, m_data.modbusAsciiLF);
    m_impl->socket.set_option(boost::asio::ip::tcp::no_delay(true), *error);              // TCP-NODELAY
    m_impl->socket.set_option(boost::asio::ip::tcp::socket::reuse_address(true), *error); // SO-REUSEADDR
    m_impl->socket.set_option(boost::asio::socket_base::keep_alive(true), *error);        // SO-KEEPALIVE
}

std::vector<uint8_t> MasterUDP::read()
{
    auto mutex = std::make_shared<std::timed_mutex>();
    mutex->lock();
    auto error = std::make_shared<boost::system::error_code>();
    auto size  = std::make_shared<std::size_t>(0);
    auto ret   = std::make_shared<std::vector<uint8_t>>(260);

    m_impl->socket.async_receive(boost::asio::buffer(*ret),
                                 [=](const boost::system::error_code& e, std::size_t s)
                                 {
                                     *size  = s;
                                     *error = e;
                                     mutex->unlock();
                                 });

    if (!mutex->try_lock_for(m_data.timeout))
    {
        m_impl->socket.cancel();
        throw std::exception("Read timeout.");
    }
    if (*error != boost::system::errc::success)
    {
        m_impl->socket.close();
        boost::asio::detail::throw_error(*error, "read");
    }
    ret->resize(*size);
    return *ret;
}

void MasterUDP::write(std::vector<uint8_t>&& data)
{
    m_impl->writeBuffer = data;
    auto mutex          = std::make_shared<std::timed_mutex>();
    mutex->lock();
    auto error = std::make_shared<boost::system::error_code>();
    m_impl->socket.async_send(boost::asio::buffer(m_impl->writeBuffer),
                              [=](const boost::system::error_code& e, std::size_t)
                              {
                                  *error = e;
                                  mutex->unlock();
                              });

    if (!mutex->try_lock_for(m_data.timeout))
    {
        m_impl->socket.cancel();
        throw std::exception("Send timeout.");
    }
    if (*error != boost::system::errc::success)
    {
        m_impl->socket.close();
        boost::asio::detail::throw_error(*error, "send");
    }
}

void MasterUDP::close() noexcept
{
    boost::system::error_code ec;
    m_impl->socket.close(ec);
}

bool MasterUDP::connected() const noexcept { return m_impl->socket.is_open(); }

} // namespace ModbusOver
