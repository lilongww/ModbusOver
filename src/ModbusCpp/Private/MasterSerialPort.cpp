﻿#include "MasterSerialPort.h"

#include <boost/asio.hpp>

namespace ModbusCpp
{
struct MasterSerialPort::Impl
{
    boost::asio::io_context io;
    boost::asio::serial_port serialPort { io };
    boost::asio::io_context::work worker { io };
    std::jthread thread;
    std::string writeBuffer;
    boost::asio::streambuf readBuffer;
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
}

std::string MasterSerialPort::read()
{
    auto mutex = std::make_shared<std::timed_mutex>();
    mutex->lock();
    auto error = std::make_shared<boost::system::error_code>();
    auto size  = std::make_shared<std::size_t>(0);
    m_impl->io.post(
        [=]()
        {
            boost::asio::async_read(m_impl->serialPort,
                                    m_impl->readBuffer,
                                    [=](const boost::system::error_code& e, std::size_t s)
                                    {
                                        *size  = s;
                                        *error = e;
                                        mutex->unlock();
                                    });
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
    std::string buffer(boost::asio::buffer_cast<const char*>(m_impl->readBuffer.data()), m_impl->readBuffer.size());
    return buffer;
}

void MasterSerialPort::write(const std::string& data)
{
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

void MasterSerialPort::setSlave(uint8_t slave) const noexcept {}

void MasterSerialPort::setTimeout(const std::chrono::milliseconds& timeout) {}

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

} // namespace ModbusCpp