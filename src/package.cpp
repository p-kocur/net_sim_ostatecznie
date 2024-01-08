//
// Created by pawel on 05.12.2023.
//
#include "package.hpp"
#include <algorithm>

std::list<ElementID> Package::assigned_IDs = {};
std::list<ElementID> Package::freed_IDs = {};


void Package::assign_ID(ElementID id) {
    auto it = std::find(freed_IDs.cbegin(), freed_IDs.cend(), id);
    if (it != freed_IDs.cend()) {
        freed_IDs.erase(it);
    }
    assigned_IDs.insert(assigned_IDs.end(), id);
}

void Package::free_ID(ElementID id) {
    auto it = std::find(assigned_IDs.cbegin(), assigned_IDs.cend(), id);
    if (it != assigned_IDs.cend()) {
        assigned_IDs.erase(it);
    }

    it = std::find(freed_IDs.cbegin(), freed_IDs.cend(), id);
    if (it == freed_IDs.cend()) {
        freed_IDs.insert(freed_IDs.end(), id);
    }
}

ElementID Package::get_new_ID() {
    if (freed_IDs.size() > 0) {
        auto it = std::min_element(freed_IDs.cbegin(), freed_IDs.cend());
        return *it;
    }
    else {
        if (assigned_IDs.size() == 0) {
            return first_index;
        }
        else {
            auto it = std::max_element(assigned_IDs.cbegin(), assigned_IDs.cend());
            return (*it) + 1;
        }
    }
}

// public

Package::Package() {
    ElementID id = get_new_ID();
    assign_ID(id);
    my_ID_ = id;
}

Package::Package(ElementID id) {
    assign_ID(id);
    my_ID_ = id;
}

Package::Package(Package &&p) {
    assign_ID(p.get_id());
    my_ID_ = p.get_id();
}

Package::~Package() {
    free_ID(get_id());
}

Package &Package::operator=(Package &&p) {
    assign_ID(p.get_id());
    my_ID_ = p.get_id();
    return *this;
}
