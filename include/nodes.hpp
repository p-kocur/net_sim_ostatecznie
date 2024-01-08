
#ifndef NODES_HPP_
#define NODES_HPP_

#include <vector>
#include <numeric>
#include <ostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include "helpers.hpp"
#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"

enum ReceiverType {
    LOADING_RAMP,
    STOREHOUSE,
    WORKER,
    LINK
};

class IPackageReceiver {
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    // dodane później
    virtual ReceiverType get_receiver_type() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual ~IPackageReceiver() = default;
};


class ReceiverPreferences {
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;
    using iterator = preferences_t::iterator;

private:
    ProbabilityGenerator my_pg;
    preferences_t my_pref;

public:
    ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : my_pg(pg){};
    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const { return my_pref; }

    iterator begin() { return my_pref.begin(); }
    iterator end() { return my_pref.end(); }
    const_iterator cbegin() const { return my_pref.cbegin(); }
    const_iterator cend() const { return my_pref.cend(); }
    const_iterator begin() const { return my_pref.begin(); }
    const_iterator end() const { return my_pref.end(); }
};


class PackageSender {
public:
    ReceiverPreferences receiver_preferences_;
    std::optional<Package> send_buffer;
    PackageSender(): send_buffer(std::nullopt) {};
    PackageSender(PackageSender&& other) = default;
    void send_package();
    const std::optional<Package>& get_sending_buffer() const { return send_buffer; };

protected:
    void push_package(Package&& package) { send_buffer.emplace(std::move(package)); };
};


class Storehouse: public IPackageReceiver {
private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
public:
    Storehouse(ElementID id, std::unique_ptr<PackageQueue> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)) : id_(id), d_(std::move(d)) {};

    void receive_package(Package&& p) override { d_->push(std::move(p)); };

    ElementID get_id() const override { return id_; };

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); };
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); };

    ReceiverType get_receiver_type() const override {return ReceiverType::STOREHOUSE;};

    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }
};



class Ramp : public PackageSender {
private:
    ElementID my_ID_;
    TimeOffset delivery_interval_;

public:
    Ramp(ElementID id, TimeOffset di) : my_ID_(id), delivery_interval_(di) { };

    void deliver_goods(Time t);

    TimeOffset get_delivery_interval() const { return delivery_interval_; };

    ElementID get_id() const { return my_ID_; };
};



class Worker : public PackageSender, public IPackageReceiver {
private:
    ElementID my_ID_;

    TimeOffset processing_duration_;

    std::optional<Package> processed_package_ = std::nullopt;

    Time package_processing_start_time_ = 0;

    std::unique_ptr<IPackageQueue> package_queue_;

public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) :
        my_ID_(id), processing_duration_(pd), package_queue_(std::move(q)) {};

    void do_work(Time t);

    TimeOffset get_processing_duration() const { return processing_duration_; };

    Time get_package_processing_start_time() const { return package_processing_start_time_; };

    bool is_processing_package() const { return processed_package_.has_value(); };

    IPackageQueue* get_queue() const { return package_queue_.get(); };

    void receive_package(Package&& p) override { package_queue_->push(std::move(p)); };
    ElementID get_id() const override { return my_ID_; };

    IPackageStockpile::const_iterator cbegin() const override { return package_queue_->cbegin(); };
    IPackageStockpile::const_iterator cend() const override { return package_queue_->cend(); };

    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; };

    std::optional<Package> const &get_processing_buffer() const { return processed_package_; }

    IPackageStockpile::const_iterator begin() const override { return package_queue_->begin(); }
    IPackageStockpile::const_iterator end() const override { return package_queue_->end(); }

};

#endif //NODES_HPP_
