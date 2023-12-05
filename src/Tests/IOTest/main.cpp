#include <ModbusOver/Master.h>
#include <gtest/gtest.h>

#include <ranges>

using namespace ModbusOver;
TEST(IOTest, bool)
{
    Master master;
    master.connect(Address<AddressType::TCP>("127.0.0.1"));
    master.setSlave(0x01);
    {                                           // 0x03
        for (auto i : std::views::iota(0, 100)) // stress
        {
            auto ret = master.readHoldingRegisters(0, 9);
            for (auto [i, r] : std::views::zip(std::views::iota(0), ret))
            {
                EXPECT_EQ(i, r);
            }
        }
    }
    {                                           // 0x04
        for (auto i : std::views::iota(0, 100)) // stress
        {
            auto data = std::views::iota(static_cast<uint16_t>(0), static_cast<uint16_t>(10)) | std::ranges::to<std::vector>();
            master.writeMultipleRegisters(0, data);
            auto ret = master.readHoldingRegisters(0, 10);
            EXPECT_EQ(data, ret);
        }
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}