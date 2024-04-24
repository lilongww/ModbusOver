#include <ModbusOver/Master.h>
#include <ModbusOver/ProtocolDebug.h>
#include <gtest/gtest.h>

#include <ranges>

using namespace ModbusOver;
TEST(IOTest, bool)
{
    Master master;
    // master.connect(Address<AddressType::TCP>("127.0.0.1"));
    master.connect(Address<AddressType::SerialPort>("COM2", ModbusProtocol::ModbusRTU));
    // master.connect(Address<AddressType::TCP>("127.0.0.1", 502, ModbusProtocol::ModbusRTU));
    // master.setProtocolDebug(std::make_shared<ProtocolDebug>());
    master.setSlave(0x01);
    // auto ret = master.reportServerID();
    // auto ret = master.readExceptionStatus(); // No test
    // auto ret = master.requestReadFIFOQueue(0); // No test
    {                                          // 离散量输入测试
        for (auto i : std::views::iota(0, 10)) // stress
        {
            auto ret = master.readDiscreteInputs(0, 10);
            EXPECT_EQ(ret.size(), 2);
            EXPECT_EQ(ret[0], 0xFF);
            EXPECT_EQ(ret[1], 0x03);
        }
    }
    {                                          // 输入寄存器测试
        for (auto i : std::views::iota(0, 10)) // stress
        {
            auto ret = master.readInputRegisters(0, 10);
            for (auto [i, r] : std::views::zip(std::views::iota(0), ret))
            {
                EXPECT_EQ(i, r);
            }
        }
    }
    {
        for (auto i : std::views::iota(0, 10)) // stress
        {
            std::vector<uint8_t> data { 0xFF, 0x03 };
            master.writeMultipleCoils(0, 10, data);
            auto ret = master.readCoils(0, 10);
            EXPECT_EQ(data, ret);
        }
    }
    {
        for (auto i : std::views::iota(0, 10)) // stress
        {
            auto data = std::views::iota(static_cast<uint16_t>(0), static_cast<uint16_t>(10)) | std::ranges::to<std::vector>();
            master.writeMultipleRegisters(0, data);
            auto ret = master.readHoldingRegisters(0, 10);
            EXPECT_EQ(data, ret);
        }
    }
    {
        for (auto i : std::views::iota(0, 10)) // stress
        {
            master.writeSingleRegister(0x05, 0xFF);
            auto ret = master.readHoldingRegisters(0x05, 1);
            EXPECT_EQ(0xFF, ret.front());
        }
    }
    {
        for (auto i : std::views::iota(0, 10)) // stress
        {
            master.writeSingleCoil(0x01, true);
            auto ret = master.readCoils(0x01, 1);
            EXPECT_EQ(0x01, ret.front());
        }
    }

    {
        for (auto i : std::views::iota(0, 10)) // stress
        {
            auto data = std::views::iota(static_cast<uint16_t>(0), static_cast<uint16_t>(10)) | std::ranges::to<std::vector>();
            auto ret  = master.readWriteMultipleRegisters(0, 10, 0, data);
            EXPECT_EQ(data, ret);
        }
    }

    {
        for (auto i : std::views::iota(0, 10)) // stress
        {
            auto data = std::views::iota(static_cast<uint16_t>(0), static_cast<uint16_t>(10)) | std::ranges::to<std::vector>();
            master.maskWriteRegister(0, 0x01, 0x02);
        }
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}