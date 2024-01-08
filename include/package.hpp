//
// Created by pawel on 06.12.2023.
//
#ifndef PACKAGE_HPP_
#define PACKAGE_HPP_

#include <vector>
#include <numeric>
#include <ostream>
#include <list>
#include "types.hpp"

class Package {
private:
    static const size_type first_index = 1;
    static std::list<ElementID> assigned_IDs;
    static std::list<ElementID> freed_IDs;
    static void assign_ID(ElementID id);
    static void free_ID(ElementID id);
    static ElementID get_new_ID();

    ElementID my_ID_;
public:
    Package();
    Package(ElementID id);
    Package(Package&& p);
    Package& operator=(Package&&);
    ElementID get_id() const { return my_ID_; }
    ~Package();
};

#endif //PACKAGE_HPP_
