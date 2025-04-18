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

namespace ModbusOver
{
class ModbusTCP : public AbstractProtocol
{
public:
    using AbstractProtocol::AbstractProtocol;
    Buffer toADU(Buffer pdu) const override;
    std::optional<BufferStream> toPDU(FunctionCode expectFunctionCode, Buffer& adu) const override;
    uint16_t aduMaximum() const override;
    uint16_t minimumSize() const override;
    ModbusProtocol proto() const override { return ModbusProtocol::ModbusTCP; }
    [[nodiscard]] bool checkTail(BufferStream&) const override { return true; }

private:
    uint16_t m_transactionIdentifier { 0 };
};
} // namespace ModbusOver