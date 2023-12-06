﻿/*********************************************************************************
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

#include <algorithm>
#include <cstdint>
#include <ranges>
#include <vector>

namespace ModbusOver
{
inline uint8_t lrc(const uint8_t* data, uint16_t size)
{
    auto total = std::ranges::fold_left(std::span(data, size), static_cast<uint8_t>(0), std::plus<uint8_t> {});
    return static_cast<uint8_t>(-static_cast<int8_t>(total));
}
} // namespace ModbusOver
