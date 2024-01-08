#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "package.hpp"
#include "nodes.hpp"

#include "types.hpp"
#include "storage_types.hpp"


#include "nodes_mocks.hpp"
#include "global_functions_mock.hpp"

#include <iostream>

using ::std::cout;
using ::std::endl;

// -----------------

TEST(WorkerTest, HasBuffer) {

    Worker w(1, 2, std::make_unique<PackageQueue>(PackageQueueType::FIFO));
    Time t = 1;

    w.receive_package(Package(1));
    w.do_work(t);
    ++t;
    w.receive_package(Package(2));
    w.do_work(t);
    auto& buffer = w.get_sending_buffer();

    ASSERT_TRUE(buffer.has_value());
    EXPECT_EQ(buffer.value().get_id(), 1);
}



TEST(RampTest, IsDeliveryOnTime) {

    Ramp r(1, 2);
    auto recv = std::make_unique<Storehouse>(1);

    r.receiver_preferences_.add_receiver(recv.get());

    r.deliver_goods(1);
    ASSERT_TRUE(r.get_sending_buffer().has_value());
    r.send_package();

    r.deliver_goods(2);
    ASSERT_FALSE(r.get_sending_buffer().has_value());

    r.deliver_goods(3);
    ASSERT_TRUE(r.get_sending_buffer().has_value());
}



TEST(ReceiverPreferencesTest, AddReceiversRescalesProbability) {
    ReceiverPreferences rp;

    MockReceiver r1;
    rp.add_receiver(&r1);
    ASSERT_NE(rp.get_preferences().find(&r1), rp.get_preferences().end());
    EXPECT_EQ(rp.get_preferences().at(&r1), 1.0);

    MockReceiver r2;
    rp.add_receiver(&r2);
    EXPECT_EQ(rp.get_preferences().at(&r1), 0.5);
    ASSERT_NE(rp.get_preferences().find(&r2), rp.get_preferences().end());
    EXPECT_EQ(rp.get_preferences().at(&r2), 0.5);
}

TEST(ReceiverPreferencesTest, RemoveReceiversRescalesProbability) {
    ReceiverPreferences rp;

    MockReceiver r1, r2;
    rp.add_receiver(&r1);
    rp.add_receiver(&r2);

    rp.remove_receiver(&r2);
    ASSERT_EQ(rp.get_preferences().find(&r2), rp.get_preferences().end());
    EXPECT_EQ(rp.get_preferences().at(&r1), 1.0);
}


using ::testing::Return;

class ReceiverPreferencesChoosingTest : public GlobalFunctionsFixture {
};

TEST_F(ReceiverPreferencesChoosingTest, ChooseReceiver) {

    EXPECT_CALL(global_functions_mock, generate_canonical()).WillOnce(Return(0.3)).WillOnce(Return(0.7));

    ReceiverPreferences rp;

    MockReceiver r1, r2;
    rp.add_receiver(&r1);
    rp.add_receiver(&r2);

    if (rp.begin()->first == &r1) {
        EXPECT_EQ(rp.choose_receiver(), &r1);
        EXPECT_EQ(rp.choose_receiver(), &r2);
    } else {
        EXPECT_EQ(rp.choose_receiver(), &r2);
        EXPECT_EQ(rp.choose_receiver(), &r1);
    }
}

// -----------------

using ::testing::Return;
using ::testing::_;


void PrintTo(const IPackageStockpile::const_iterator& it, ::std::ostream* os) {
    *os << it->get_id();
}

class PackageSenderFixture : public PackageSender {
public:
    void push_package(Package&& package) { PackageSender::push_package(std::move(package)); }
};


TEST(PackageSenderTest, SendPackage) {
    MockReceiver mock_receiver;
    // Oczekujemy, że metoda `receive_package()` obiektu `mock_receiver` zostanie
    // wywołana dwukrotnie, z dowolnym argumentem (symbol `_`).
    EXPECT_CALL(mock_receiver, receive_package(_)).Times(1);

    PackageSenderFixture sender;
    sender.receiver_preferences_.add_receiver(&mock_receiver);
    sender.push_package(Package());

    sender.send_package();

    EXPECT_FALSE(sender.get_sending_buffer());

    sender.send_package();
}
