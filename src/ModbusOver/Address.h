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
#pragma once

#include "Standard.h"

#include <string>

namespace ModbusOver
{
template<AddressType addr>
class Address
{
    static_assert(sizeof(addr), "Unknown visa address");
};

class AddressBase
{
public:
    inline AddressBase(ModbusProtocol proto) : m_protocol(proto) {}
    inline ModbusProtocol protocol() const { return m_protocol; }

private:
    ModbusProtocol m_protocol;
};

template<>
class Address<AddressType::TCP> : public AddressBase
{
public:
    constexpr static auto DefaultPort = 502;
    inline Address(std::string_view ip, uint16_t port = DefaultPort, ModbusProtocol proto = ModbusProtocol::ModbusTCP)
        : AddressBase(proto), m_ip(ip), m_port(port)
    {
    }
    inline const std::string& ip() const { return m_ip; }
    inline uint16_t port() const { return m_port; }

private:
    std::string m_ip;
    uint16_t m_port;
};

template<>
class Address<AddressType::UDP> : public AddressBase
{
public:
    inline Address(std::string_view ip, uint16_t port, ModbusProtocol proto = ModbusProtocol::ModbusTCP)
        : AddressBase(proto), m_ip(ip), m_port(port)
    {
    }
    inline const std::string& ip() const { return m_ip; }
    inline uint16_t port() const { return m_port; }

private:
    std::string m_ip;
    uint16_t m_port;
};

enum BaudRate
{
    _1200   = 1200,
    _2400   = 2400,
    _4800   = 4800,
    _9600   = 9600,
    _19200  = 19200,
    _38400  = 38400,
    _57600  = 57600,
    _115200 = 115200
};

enum class DataBits
{
    Data5 = 5,
    Data6,
    Data7,
    Data8
};

enum class FlowControl
{
    None,
    Software,
    Hardware
};

enum class Parity
{
    None,
    Odd,
    Even
};

enum class StopBits
{
    One,
    OnePointFive,
    Two
};

template<>
class Address<AddressType::SerialPort> : public AddressBase
{
public:
    inline Address(std::string_view portName,
                   uint32_t baudRate,
                   DataBits dataBits       = DataBits::Data8,
                   FlowControl flowControl = FlowControl::None,
                   Parity parity           = Parity::None,
                   StopBits stopBits       = StopBits::One,
                   ModbusProtocol proto    = ModbusProtocol::ModbusRTU)
        : AddressBase(proto)
        , m_portName(portName)
        , m_baudRate(baudRate)
        , m_dataBits(dataBits)
        , m_flowControl(flowControl)
        , m_parity(parity)
        , m_stopBits(stopBits)
    {
    }
    inline Address(std::string_view port, ModbusProtocol proto = ModbusProtocol::ModbusRTU)
        : Address(port, BaudRate::_9600, DataBits::Data8, FlowControl::None, Parity::Even, StopBits::One, proto)
    {
    }

    inline const std::string& portName() const { return m_portName; }
    inline uint32_t baudRate() const { return m_baudRate; }
    inline DataBits dataBits() const { return m_dataBits; }
    inline FlowControl flowControl() const { return m_flowControl; }
    inline Parity parity() const { return m_parity; }
    inline StopBits stopBits() const { return m_stopBits; }

private:
    std::string m_portName;
    uint32_t m_baudRate { 9600 };
    DataBits m_dataBits { DataBits::Data8 };
    FlowControl m_flowControl { FlowControl::None };
    Parity m_parity { Parity::None };
    StopBits m_stopBits { StopBits::One };
};

template<typename T>
constexpr bool _IsAddress = std::false_type {};
template<AddressType T>
constexpr bool _IsAddress<Address<T>> = std::true_type {};
template<typename T>
concept IsAddress = _IsAddress<T>;

} // namespace ModbusOver