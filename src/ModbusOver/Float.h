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

#include <bit>

namespace ModbusOver
{
struct ABCD
{
    uint8_t a, b, c, d;
};

constexpr _toABCD(const uint16_t* src)
{
    return ABCD { (src[0] >> 8) & 0xFF, (src[0] >> 0) & 0xFF, (src[1] >> 8) & 0xFF, (src[1] >> 0) & 0xFF };
}

constexpr float toFloatABCD(const uint16_t* src)
{
    auto [a, b, c, d] = _toABCD(src);
    uint32_t i        = (a << 24) | (b << 16) | (c << 8) | (d << 0);
    return std::bit_cast<float>(i);
}

constexpr float toFloatDCBA(const uint16_t* src)
{
    auto [a, b, c, d] = _toABCD(src);
    uint32_t i        = (d << 24) | (c << 16) | (b << 8) | (a << 0);
    return std::bit_cast<float>(i);
}

constexpr float toFloatDABC(const uint16_t* src)
{
    auto [a, b, c, d] = _toABCD(src);
    uint32_t i        = (b << 24) | (a << 16) | (d << 8) | (c << 0);
    return std::bit_cast<float>(i);
}

constexpr float toFloatCDAB(const uint16_t* src)
{
    auto [a, b, c, d] = _toABCD(src);
    uint32_t i        = (c << 24) | (d << 16) | (a << 8) | (b << 0);
    return std::bit_cast<float>(i);
}

constexpr void fromFloatABCD(float f, uint16_t* dest)
{
    uint32_t i   = std::bit_cast<uint32_t>(f);
    uint8_t* out = reinterpret_cast<uint8_t*>(dest);
    uint8_t a    = (i >> 24) & 0xFF;
    uint8_t b    = (i >> 16) & 0xFF;
    uint8_t c    = (i >> 8) & 0xFF;
    uint8_t d    = (i >> 0) & 0xFF;
    out[0]       = a;
    out[1]       = b;
    out[2]       = c;
    out[3]       = d;
}

constexpr void fromFloatDCBA(float f, uint16_t* dest)
{
    uint32_t i   = std::bit_cast<uint32_t>(f);
    uint8_t* out = reinterpret_cast<uint8_t*>(dest);
    uint8_t a    = (i >> 24) & 0xFF;
    uint8_t b    = (i >> 16) & 0xFF;
    uint8_t c    = (i >> 8) & 0xFF;
    uint8_t d    = (i >> 0) & 0xFF;
    out[0]       = d;
    out[1]       = c;
    out[2]       = b;
    out[3]       = a;
}

constexpr void fromFloatBADC(float f, uint16_t* dest)
{
    uint32_t i   = std::bit_cast<uint32_t>(f);
    uint8_t* out = reinterpret_cast<uint8_t*>(dest);
    uint8_t a    = (i >> 24) & 0xFF;
    uint8_t b    = (i >> 16) & 0xFF;
    uint8_t c    = (i >> 8) & 0xFF;
    uint8_t d    = (i >> 0) & 0xFF;
    out[0]       = b;
    out[1]       = a;
    out[2]       = d;
    out[3]       = c;
}

constexpr void fromFloatCDAB(float f, uint16_t* dest)
{
    uint32_t i   = std::bit_cast<uint32_t>(f);
    uint8_t* out = reinterpret_cast<uint8_t*>(dest);
    uint8_t a    = (i >> 24) & 0xFF;
    uint8_t b    = (i >> 16) & 0xFF;
    uint8_t c    = (i >> 8) & 0xFF;
    uint8_t d    = (i >> 0) & 0xFF;
    out[0]       = c;
    out[1]       = d;
    out[2]       = a;
    out[3]       = b;
}

} // namespace ModbusOver