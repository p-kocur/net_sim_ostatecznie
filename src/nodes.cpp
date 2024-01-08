
#include "nodes.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver* r) {
    my_pref[r] = 0;

    double probability = 1 / ((double) my_pref.size());

    for(auto& receiver : my_pref) {
        receiver.second = probability;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    my_pref.erase(r);

    double probability_sum = 0.0;

    for(auto& receiver : my_pref){
        probability_sum += receiver.second;
    }

    for(auto& receiver : my_pref){
        receiver.second = receiver.second / probability_sum;
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double p = my_pg();

    double probability_sum = 0.0;


    for(auto& receiver : my_pref) {
        probability_sum += receiver.second;

        if(probability_sum >= p) {
            return receiver.first;
        }
    }
    return nullptr;
}

void PackageSender::send_package() {
    if(send_buffer.has_value()) {
        auto receiver = receiver_preferences_.choose_receiver();

        if(receiver != nullptr) {

            receiver->receive_package(std::move(*send_buffer));

            send_buffer.reset();
        }
    }
}

void Ramp::deliver_goods(Time t) {
    if ((t - 1) % get_delivery_interval() == 0) {
        Package p = Package();
        send_buffer.emplace(std::move(p));
    }
}

void Worker::do_work(Time t) {
    if (is_processing_package()) {
        if (t > get_processing_duration() + get_package_processing_start_time() - 1) {
            send_buffer.emplace(std::move(processed_package_.value()));
            processed_package_.reset();
        }
    }
    if (!is_processing_package() && !package_queue_->empty()) {
        Package p = package_queue_->pop();
        if (get_processing_duration() <= 1) {
            send_buffer.emplace(std::move(p));
        }
        else {
            processed_package_.emplace(std::move(p));
        }
    }
}
