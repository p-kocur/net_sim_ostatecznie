
#include "storage_types.hpp"

bool PackageQueue::empty() {
    if (queue_.empty()){
        return true;
    }
    else {
        return false;
    }
}

Package PackageQueue::pop() {
    if (get_queue_type() == FIFO) {
        Package popped_elem = queue_.front().get_id();
        queue_.pop_front();
        return popped_elem;
    }
    if (get_queue_type() == LIFO) {
        Package popped_elem = queue_.back().get_id();
        queue_.pop_back();
        return popped_elem;
    }
    else {
        throw std::runtime_error("Nieznany rodzaj kolejki");
    }
}
