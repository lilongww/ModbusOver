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

#include <iterator>
#include <type_traits>

namespace ModbusOver
{
template<typename T>
class ArithmeticView
{
public:
    constexpr ArithmeticView(const T& value) : m_data(reinterpret_cast<const uint8_t*>(&value)) {}
    constexpr ~ArithmeticView() {}
    constexpr size_t size() const { return sizeof(T); }
    constexpr auto begin() const { return m_data; }
    constexpr auto end() const { return m_data + size(); }

private:
    const uint8_t* m_data;
};
} // namespace ModbusOver