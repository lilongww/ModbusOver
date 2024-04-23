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

#include "AbstractProtocol.h"
#include "BufferStream.h"

#include <cstdlib>

namespace ModbusOver
{
class Common
{
public:
    inline Common() {}
    inline Common(AbstractProtocol::FunctionCode functionCode) : m_functionCode(static_cast<uint8_t>(functionCode)) {}
    inline AbstractProtocol::FunctionCode functionCode() const { return static_cast<AbstractProtocol::FunctionCode>(m_functionCode); }
    template<typename T>
    inline Buffer encode(this T&& self);
    template<typename T>
    inline bool decode(this T& self, BufferStream& buffer);

private:
    uint8_t m_functionCode;
};

template<typename T>
inline Buffer Common::encode(this T&& self)
{
    Buffer buffer { self.m_functionCode };
    self.serialize(buffer);
    return buffer;
}

template<typename T>
bool Common::decode(this T& self, BufferStream& stream)
{
    if (stream.size() < 2)
        return false;
    stream >> self.m_functionCode;
    return self.unserialize(stream);
}

template<AbstractProtocol::FunctionCode code>
struct Codec
{
    static_assert(sizeof(code), "Unrealized code");
};

template<>
struct Codec<AbstractProtocol::FunctionCode::ReadColis>
{
    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::ReadColis>
    class Request : public Common
    {
    public:
        inline Request() {}
        inline Request(uint16_t startingAddress, uint16_t quantityOfCoils)
            : Common(code), m_startingAddress(startingAddress), m_quantityOfCoils(quantityOfCoils)
        {
        }
        inline uint16_t startingAddress() const { return m_startingAddress; }
        inline uint16_t quantityOfCoils() const { return m_quantityOfCoils; }

        inline void serialize(Buffer& buffer) const
        {
            buffer.append(m_startingAddress);
            buffer.append(m_quantityOfCoils);
        }
        inline bool unserialize(BufferStream& stream)
        {
            if (stream.size() < 4)
                return false;
            stream >> m_startingAddress >> m_quantityOfCoils;
            return true;
        }

    private:
        uint16_t m_startingAddress;
        uint16_t m_quantityOfCoils;
    };

    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::ReadColis>
    class Response : public Common
    {
    public:
        inline Response() {}
        inline Response(std::vector<uint8_t>&& data) : Common(code), m_coilStatus(std::move(data)) {}
        inline const std::vector<uint8_t>& coilStatus() const& { return m_coilStatus; }
        inline std::vector<uint8_t> coilStatus() const&& { return std::move(m_coilStatus); }
        inline void serialize(Buffer& buffer) const
        {
            buffer.append(static_cast<uint8_t>(m_coilStatus.size()));
            for (auto state : m_coilStatus)
            {
                buffer.append(state);
            }
        }
        inline bool unserialize(BufferStream& stream)
        {
            if (stream.size() < 2) // byte count + at last 1 status = 2
                return false;
            uint8_t byteCount;
            stream >> byteCount;
            if (stream.size() < byteCount)
                return false;
            m_coilStatus.resize(byteCount);
            for (auto i : std::views::iota(static_cast<uint8_t>(0), byteCount))
            {
                stream >> m_coilStatus[i];
            }
            return true;
        }

    private:
        std::vector<uint8_t> m_coilStatus;
    };
};

template<>
struct Codec<AbstractProtocol::FunctionCode::ReadDiscreteInputs>
{
    using Request  = Codec<AbstractProtocol::FunctionCode::ReadColis>::Request<AbstractProtocol::FunctionCode::ReadDiscreteInputs>;
    using Response = Codec<AbstractProtocol::FunctionCode::ReadColis>::Response<AbstractProtocol::FunctionCode::ReadDiscreteInputs>;
};

template<>
struct Codec<AbstractProtocol::FunctionCode::ReadHoldingRegisters>
{
    using Request  = Codec<AbstractProtocol::FunctionCode::ReadColis>::Request<AbstractProtocol::FunctionCode::ReadHoldingRegisters>;
    using Response = Codec<AbstractProtocol::FunctionCode::ReadColis>::Response<AbstractProtocol::FunctionCode::ReadHoldingRegisters>;
};

template<>
struct Codec<AbstractProtocol::FunctionCode::ReadInputRegisters>
{
    using Request  = Codec<AbstractProtocol::FunctionCode::ReadColis>::Request<AbstractProtocol::FunctionCode::ReadInputRegisters>;
    using Response = Codec<AbstractProtocol::FunctionCode::ReadColis>::Response<AbstractProtocol::FunctionCode::ReadInputRegisters>;
};

template<>
struct Codec<AbstractProtocol::FunctionCode::WriteSingleCoil>
{
    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::WriteSingleCoil>
    class Request : public Common
    {
    public:
        inline Request() {}
        inline Request(uint16_t address, bool on) : Common(code), m_address(address), m_value(on ? 0xFF00 : 0x0000) {}
        inline bool state() const { return static_cast<bool>(m_value); }
        inline void serialize(Buffer& buffer) const
        {
            buffer.append(m_address);
            buffer.append(m_value);
        }
        inline bool unserialize(BufferStream& stream)
        {
            if (stream.size() < 4)
                return false;
            stream >> m_address >> m_value;
            return true;
        }

    protected:
        inline Request(uint16_t address, uint16_t value) : Common(code), m_address(address), m_value(value) {}

    protected:
        uint16_t m_address;
        uint16_t m_value;
    };
    using Response = Request<AbstractProtocol::FunctionCode::WriteSingleCoil>;
};

template<>
struct Codec<AbstractProtocol::FunctionCode::WriteSingleRegister>
{
    class Request
        : public Codec<AbstractProtocol::FunctionCode::WriteSingleCoil>::Request<AbstractProtocol::FunctionCode::WriteSingleRegister>
    {
    public:
        inline Request() {}
        inline Request(uint16_t address, uint16_t value)
            : Codec<AbstractProtocol::FunctionCode::WriteSingleCoil>::Request<AbstractProtocol::FunctionCode::WriteSingleRegister>(address,
                                                                                                                                   value)
        {
        }
        inline uint16_t value() const { return m_value; }

    protected:
        using Codec<AbstractProtocol::FunctionCode::WriteSingleCoil>::Request<AbstractProtocol::FunctionCode::WriteSingleRegister>::state;
    };
    using Response = Request;
};

template<>
struct Codec<AbstractProtocol::FunctionCode::ReadExceptionStatus>
{
    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::ReadExceptionStatus>
    class Request : public Common
    {
    public:
        inline Request() : Common(code) {}
        inline void serialize(Buffer& buffer) const {}
        inline bool unserialize(BufferStream& stream) { return true; }
    };

    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::ReadExceptionStatus>
    class Response : public Common
    {
    public:
        inline Response() {}
        inline Response(uint8_t outputData) : Common(code), m_outputData(outputData) {}
        inline void serialize(Buffer& buffer) const { buffer.append(m_outputData); }
        inline bool unserialize(BufferStream& stream)
        {
            if (stream.size() < 1)
                return false;
            stream >> m_outputData;
            return true;
        }
        inline uint8_t outputData() const { return m_outputData; }

    private:
        uint8_t m_outputData;
    };
};

template<>
struct Codec<AbstractProtocol::FunctionCode::WriteMultipleCoils>
{
    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::WriteMultipleCoils>
    class Request : public Common
    {
    public:
        inline Request() {}
        inline Request(uint16_t startingAddress, uint16_t quantityOfOutputs, std::vector<uint8_t>&& states)
            : Common(code), m_startingAddress(startingAddress), m_quantityOfOutputs(quantityOfOutputs), m_states(std::move(states))
        {
        }
        inline void serialize(Buffer& buffer) const
        {
            buffer.append(m_startingAddress);
            buffer.append(m_quantityOfOutputs);
            uint8_t byteCount = static_cast<uint8_t>(m_states.size());
            buffer.append(byteCount);
            for (auto state : m_states)
            {
                buffer.append(state);
            }
        }
        inline bool unserialize(BufferStream& stream)
        {
            if (stream.size() < 5)
                return false;
            uint8_t size;
            stream >> m_startingAddress >> m_quantityOfOutputs >> size;
            if (stream.size() < size)
                return false;
            m_states.resize(size);
            for (auto i : std::views::iota(static_cast<uint8_t>(0), size))
            {
                stream >> m_states[i];
            }
            return true;
        }

    private:
        uint16_t m_startingAddress;
        uint16_t m_quantityOfOutputs;
        std::vector<uint8_t> m_states;
    };

    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::WriteMultipleCoils>
    class Response : public Common
    {
    public:
        inline Response() {}
        inline Response(uint16_t startingAddress, uint16_t quantityOfOutputs)
            : Common(code), m_startingAddress(startingAddress), m_quantityOfOutputs(quantityOfOutputs)
        {
        }
        inline void serialize(Buffer& buffer) const
        {
            buffer.append(m_startingAddress);
            buffer.append(m_quantityOfOutputs);
        }
        inline bool unserialize(BufferStream& stream)
        {
            if (stream.size() < 4)
                return false;
            stream >> m_startingAddress >> m_quantityOfOutputs;
            return true;
        }

    private:
        uint16_t m_startingAddress;
        uint16_t m_quantityOfOutputs;
    };
};

template<>
struct Codec<AbstractProtocol::FunctionCode::WriteMultipleRegisters>
{
    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::WriteMultipleRegisters>
    class Request : public Common
    {
    public:
        inline Request() {}
        inline Request(uint16_t startingAddress, uint16_t quantityOfRegisters, std::vector<uint16_t>&& values)
            : Common(code), m_startingAddress(startingAddress), m_quantityOfRegisters(quantityOfRegisters), m_values(std::move(values))
        {
        }
        inline void serialize(Buffer& buffer) const
        {
            buffer.append(m_startingAddress);
            buffer.append(m_quantityOfRegisters);
            uint8_t byteCount = static_cast<uint8_t>(m_values.size() * sizeof(uint16_t));
            buffer.append(byteCount);
            for (auto value : m_values)
            {
                buffer.append(value);
            }
        }
        inline bool unserialize(BufferStream& stream)
        {
            if (stream.size() < 5)
                return false;
            uint8_t size;
            stream >> m_startingAddress >> m_quantityOfRegisters >> size;
            if (stream.size() < size)
                return false;
            m_values.resize(size);
            for (auto i : std::views::iota(static_cast<uint8_t>(0), size))
            {
                stream >> m_values[i];
            }
            return true;
        }

    private:
        uint16_t m_startingAddress;
        uint16_t m_quantityOfRegisters;
        std::vector<uint16_t> m_values;
    };
    using Response =
        Codec<AbstractProtocol::FunctionCode::WriteMultipleCoils>::Response<AbstractProtocol::FunctionCode::WriteMultipleRegisters>;
};

template<>
struct Codec<AbstractProtocol::FunctionCode::ReportServerID>
{
    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::ReportServerID>
    class Request : public Common
    {
    public:
        inline Request() : Common(code) {}
        inline void serialize(Buffer& buffer) const {}
        inline bool unserialize(BufferStream& stream) { return true; }
    };

    template<AbstractProtocol::FunctionCode code = AbstractProtocol::FunctionCode::ReportServerID>
    class Response : public Common
    {
    public:
        inline Response() {}
        inline Response(std::vector<uint8_t>&& data) : Common(code), m_byteCount(static_cast<uint8_t>(data.size())), m_data(std::move(data))
        {
        }
        inline void serialize(Buffer& buffer) const
        {
            buffer.append(m_byteCount);
            for (auto data : m_data)
            {
                buffer.append(data);
            }
        }
        inline bool unserialize(BufferStream& stream)
        {
            if (stream.size() < 2)
                return false;
            stream.peak(m_byteCount);
            if (stream.size() < m_byteCount)
                return false;
            stream >> m_byteCount;
            m_data.resize(m_byteCount);
            for (auto& d : m_data)
            {
                stream >> d;
            }
            return true;
        }
        inline operator std::vector<uint8_t>() && { return std::move(m_data); }

    private:
        uint8_t m_byteCount;
        std::vector<uint8_t> m_data;
    };
};
} // namespace ModbusOver