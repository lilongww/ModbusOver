/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023 LiLong                                                   **
**  This file is part of ModbusOver.                                             **
**                                                                              **
**  ModbusOver is free software: you can redistribute it and/or modify           **
**  it under the terms of the GNU Lesser General Public License as published by **
**  the Free Software Foundation, either version 3 of the License, or           **
**  (at your option) any later version.                                         **
**                                                                              **
**  ModbusOver is distributed in the hope that it will be useful,                **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of              **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               **
**  GNU Lesser General Public License for more details.                         **
**                                                                              **
**  You should have received a copy of the GNU Lesser General Public License    **
**  along with ModbusOver.  If not, see <https://www.gnu.org/licenses/>.         **
**********************************************************************************/
#pragma once

#include "ArithmeticView.h"
#include "MBAP.h"

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <span>
#include <vector>

namespace ModbusOver
{

class Buffer
{
public:
    inline Buffer() {}
    template<typename... Args>
    inline Buffer(const Args&... args);
    inline Buffer(Buffer&& other) : m_data(std::move(other.m_data)) {}
    inline Buffer(std::vector<uint8_t>&& data) : m_data(std::move(data)) {}
    inline Buffer(const Buffer& other) = delete;
    template<typename Self>
    inline auto&& data(this Self&& self)
    {
        return std::forward<Self>(self).m_data;
    }
    template<typename T>
    requires std::is_arithmetic_v<T> || std::is_enum_v<T>
    inline void append(const T& value);
    inline void appendCrc(uint16_t value) { std::ranges::copy(ArithmeticView(value), std::back_inserter(m_data)); }
    inline size_t size() const { return m_data.size(); }
    template<typename T>
    requires std::is_arithmetic_v<T> || std::is_enum_v<T>
    inline void prepend(const T& value);

private:
    std::vector<uint8_t> m_data;
};

template<typename... Args>
inline Buffer::Buffer(const Args&... args)
{
    (append(args), ...);
}

template<typename T>
requires std::is_arithmetic_v<T> || std::is_enum_v<T>
inline void Buffer::append(const T& value)
{
    std::ranges::reverse_copy(ArithmeticView(value), std::back_inserter(m_data));
}

template<typename T>
requires std::is_arithmetic_v<T> || std::is_enum_v<T>
void Buffer::prepend(const T& value)
{
    m_data.insert_range(m_data.begin(), ArithmeticView(value) | std::views::reverse);
}

} // namespace ModbusOver