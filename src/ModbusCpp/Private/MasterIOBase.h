/*********************************************************************************
**                                                                              **
**  Copyright (C) 2023 LiLong                                                   **
**  This file is part of ModbusCpp.                                             **
**                                                                              **
**  ModbusCpp is free software: you can redistribute it and/or modify           **
**  it under the terms of the GNU Lesser General Public License as published by **
**  the Free Software Foundation, either version 3 of the License, or           **
**  (at your option) any later version.                                         **
**                                                                              **
**  ModbusCpp is distributed in the hope that it will be useful,                **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of              **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               **
**  GNU Lesser General Public License for more details.                         **
**                                                                              **
**  You should have received a copy of the GNU Lesser General Public License    **
**  along with ModbusCpp.  If not, see <https://www.gnu.org/licenses/>.         **
**********************************************************************************/
#pragma once

#include "../Standard.h"
#include "CommonData.h"

#include <chrono>
#include <string>

namespace ModbusCpp
{
class MasterIOBase
{
public:
    MasterIOBase(const MasterCommonData& data) : m_data(data) {}
    virtual ~MasterIOBase() noexcept                                  = default;
    virtual std::string read()                                        = 0;
    virtual void write(const std::string& data)                       = 0;
    virtual void close() noexcept                                     = 0;
    virtual bool connected() const noexcept                           = 0;
    virtual void setSlave(uint8_t slave) const noexcept               = 0;
    virtual void setTimeout(const std::chrono::milliseconds& timeout) = 0;

protected:
    const MasterCommonData& m_data;
};
} // namespace ModbusCpp