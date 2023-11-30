#include "MasterSerialPort.h"
#include "AbstractProtocol.h"
#include "Buffer.h"

#include <boost/asio.hpp>

namespace ModbusCpp
{
struct MasterSerialPort::Impl
{
    boost::asio::io_context io;
    boost::asio::serial_port serialPort { io };
    boost::asio::io_context::work worker { io };
    std::jthread thread;
    std::vector<uint8_t> writeBuffer;
    boost::asio::streambuf readBuffer;
    std::shared_ptr<AbstractProtocol> protocol;
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
    m_impl->protocol = AbstractProtocol::create(address.protocol(), m_data.slave);
}

std::vector<uint8_t> MasterSerialPort::read()
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
    auto data = boost::asio::buffer_cast<const uint8_t*>(m_impl->readBuffer.data());
    std::vector<uint8_t> buffer(data, data + m_impl->readBuffer.size());
    return buffer;
}

void MasterSerialPort::write(std::vector<uint8_t>&& data)
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

std::vector<uint8_t> MasterSerialPort::readCoils(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    write(m_impl->protocol->requestReadColis(startingAddress, quantityOfCoils).data());
    std::vector<uint8_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_impl->protocol->onResponseReadColis(buffer, ret))
            return ret;
    }
}

std::vector<boost::uint8_t> MasterSerialPort::readDiscreteInputs(uint16_t startingAddress, uint16_t quantityOfCoils)
{
    write(m_impl->protocol->requestReadDiscreteInputs(startingAddress, quantityOfCoils).data());
    std::vector<uint8_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_impl->protocol->onResponseReadDiscreteInputs(buffer, ret))
            return ret;
    }
}

std::vector<uint16_t> MasterSerialPort::readHoldingRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    write(m_impl->protocol->requestReadHoldingRegisters(startingAddress, quantityOfRegisters).data());
    std::vector<uint16_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_impl->protocol->onResponseReadHoldingRegisters(buffer, ret))
            return ret;
    }
}

std::vector<uint16_t> MasterSerialPort::readInputRegisters(uint16_t startingAddress, uint16_t quantityOfRegisters)
{
    write(m_impl->protocol->requestReadInputRegisters(startingAddress, quantityOfRegisters).data());
    std::vector<uint16_t> ret;
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_impl->protocol->onResponseReadInputRegisters(buffer, ret))
            return ret;
    }
}

void MasterSerialPort::writeSingleCoil(uint16_t address, bool on)
{
    write(m_impl->protocol->requestWriteSingleCoil(address, on).data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_impl->protocol->onResponseWriteSingleCoil(buffer))
            return;
    }
}

void MasterSerialPort::writeSingleRegister(uint16_t address, uint16_t value)
{
    write(m_impl->protocol->requestWriteSingleRegister(address, value).data());
    Buffer buffer;
    for (;;)
    {
        buffer.data().append_range(read());
        if (m_impl->protocol->onResponseWriteSingleRegister(buffer))
            return;
    }
}

} // namespace ModbusCpp