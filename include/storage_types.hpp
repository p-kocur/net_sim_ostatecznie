
#ifndef STORAGE_TYPES_HPP_
#define STORAGE_TYPES_HPP_

#include <list>
#include "types.hpp"
#include "package.hpp"

enum PackageQueueType {
    FIFO,
    LIFO
};

class IPackageStockpile {
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&&) = 0;

    virtual bool empty() = 0;

    virtual size_type size() = 0;


    virtual const_iterator begin() = 0;
    virtual const_iterator end() = 0;
    virtual const_iterator cbegin() = 0;
    virtual const_iterator cend() = 0;


    virtual ~IPackageStockpile() = default;
};

class IPackageQueue: public IPackageStockpile {
public:

    virtual Package pop() = 0;

    virtual PackageQueueType get_queue_type() = 0;

    ~IPackageQueue() = default;
};


class PackageQueue: public IPackageQueue {
private:
    std::list<Package> queue_;
    PackageQueueType queue_type_;
public:

    PackageQueue(PackageQueueType t) : queue_type_(t) {};
    bool empty() override;
    size_type size() override {return queue_.size(); };
    PackageQueueType get_queue_type() override {return queue_type_; };
    void push(Package&& p) override { queue_.push_back(std::move(p)); };
    Package pop() override;


    const_iterator begin() override { return queue_.begin(); };
    const_iterator end() override { return queue_.end(); };
    const_iterator cbegin() override { return queue_.cbegin(); };
    const_iterator cend() override { return queue_.cend(); };
};

#endif //STORAGE_TYPES_HPP_
