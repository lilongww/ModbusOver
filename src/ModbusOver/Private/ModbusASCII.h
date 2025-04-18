﻿/*********************************************************************************
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
#include "Buffer.h"

namespace ModbusOver
{
class ModbusASCII : public AbstractProtocol
{
public:
    inline ModbusASCII(const uint8_t& slave, const char& asciiLF) : AbstractProtocol(slave), m_asciiLF(asciiLF) {}
    Buffer toADU(Buffer pdu) const override;
    std::optional<BufferStream> toPDU(FunctionCode expectFunctionCode, Buffer& adu) const override;
    uint16_t aduMaximum() const override;
    uint16_t minimumSize() const override;
    ModbusProtocol proto() const override { return ModbusProtocol::ModbusASCII; }
    [[nodiscard]] bool checkTail(BufferStream& stream) const override;

private:
    const char& m_asciiLF;
};
} // namespace ModbusOver