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

#pragma once

#include "ArithmeticView.h"
#include "Buffer.h"

#include <ranges>

namespace ModbusOver
{
class BufferStream
{
public:
    inline BufferStream(const Buffer& buffer, size_t offset = 0) : m_data(buffer.data()), m_offset(offset) {}
    inline size_t size() const { return m_data.size() - m_offset; }
    template<typename T>
    inline BufferStream& operator>>(T& value)
    {
        peak(value);
        m_offset += sizeof(value);
        return *this;
    }
    inline uint16_t msbCrc() const { return std::byteswap(*reinterpret_cast<const uint16_t*>(&m_data.at(m_data.size() - 2))); }
    inline uint16_t lsbCrc() const { return *reinterpret_cast<const uint16_t*>(&m_data.at(m_data.size() - 2)); }
    template<typename T>
    inline void peak(T& value)
    {
        std::ranges::reverse_copy(m_data | std::views::drop(m_offset) | std::views::take(sizeof(T)), reinterpret_cast<int8_t*>(&value));
    }

private:
    const std::vector<uint8_t>& m_data;
    size_t m_offset;
};
} // namespace ModbusOver