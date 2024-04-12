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
#include "ProtocolDebug.h"

#include <format>
#include <print>

template<class T, class CharT>
struct std::formatter<std::vector<T>, CharT> : std::formatter<T, CharT>
{
    template<class FormatContext>
    auto format(const std::vector<T>& t, FormatContext& fc) const
    {
        std::format_to(fc.out(), "[");
        for (auto it = t.begin(); it != t.end(); ++it)
        {
            if (it + 1 != t.end())
                std::format_to(std::formatter<T, CharT>::format(*it, fc), ",");
            else
                std::formatter<T, CharT>::format(*it, fc);
        }
        return std::format_to(fc.out(), "]");
    }
};

namespace ModbusOver
{
/*!
    \class      ModbusOver::ProtocolDebug
    \brief      协议调试接口，提供默认的按16进制数组打印到标准输出的方法，若需自己处理，则继承此类型并实现rx和tx方法.
    \ingroup    ModbusOver
    \inmodule   ModbusOver
    \inheaderfile ProtocolDebug.h
*/

/*!
    \brief      监视发送数据 \a data.
    \sa         rx
*/
void ProtocolDebug::tx(const std::vector<uint8_t>& data) { std::print("Tx:{:02X}\n", data); }

/*!
    \brief      监视接收数据 \a data.
    \sa         tx
*/
void ProtocolDebug::rx(const std::vector<uint8_t>& data) { std::print("Rx:{:02X}\n", data); }
} // namespace ModbusOver