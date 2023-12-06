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

#include "AbstractProtocol.h"

namespace ModbusOver
{
class ModbusRTU : public AbstractProtocol
{
public:
    inline ModbusRTU(const uint8_t& slave, const bool& useBigendianCRC16) : AbstractProtocol(slave), m_useBigendianCRC16(useBigendianCRC16)
    {
    }
    Buffer toADU(Buffer pdu) const override;
    std::optional<BufferStream> toPDU(FunctionCode expectFunctionCode, Buffer& adu) const override;
    uint16_t aduMaximum() const override;
    uint16_t minimumSize() const override;
    ModbusProtocol proto() const override { return ModbusProtocol::ModbusRTU; }

private:
    const bool& m_useBigendianCRC16;
};
} // namespace ModbusOver