#include <ModbusOver/Master.h>
#include <gtest/gtest.h>

#include <ranges>

using namespace ModbusOver;
TEST(IOTest, bool)
{
    Master master;
    master.connect(Address<AddressType::TCP>("127.0.0.1"));
    master.setSlave(0x01);
    { // 0x03
        auto ret = master.readHoldingRegisters(0, 9);
        for (auto [i, r] : std::views::zip(std::views::iota(0), ret))
        {
            EXPECT_EQ(i, r);
        }
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}