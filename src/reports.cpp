
#include <iostream>
#include "reports.hpp"

void generate_structure_report(const Factory &f, std::ostream  &os)
{
    os << "\n" << "== LOADING RAMPS ==" << "\n" << "\n";
    auto it_r = f.ramp_cbegin();
    ElementID id = 1;
    while (it_r != f.ramp_cend())
    {
        for (auto it = f.ramp_cbegin(); it != f.ramp_cend(); it++)
        {
            const auto &ramp = *it;
            if (id == ramp.get_id())
            {
                os << "LOADING RAMP #" << ramp.get_id() << "\n" << "  Delivery interval: "
                   << ramp.get_delivery_interval()
                   << "\n";
                os << "  Receivers:" << "\n";
                auto sorted_receivers = sort_map(ramp.receiver_preferences_);
                for (auto &receiver: sorted_receivers)
                {
                    if (receiver.first->get_receiver_type() == WORKER)
                    {
                        os << "    worker #" << receiver.first->get_id() << "\n";
                    }
                    else
                    {
                        os << "    storehouse #" << receiver.first->get_id() << "\n";
                    }
                }
                os << "\n";
                ++it_r;
                break;
            }
        }
        ++id;
    }
    os << "\n" << "== WORKERS ==" << "\n" << "\n";
    auto it_w = f.worker_cbegin();
    id = 1;
    while (it_w != f.worker_cend())
    {
        for(auto it = f.worker_cbegin(); it != f.worker_cend(); it++)
        {
            const auto& worker = *it;
            if (id == worker.get_id())
            {
                os << "WORKER #" << worker.get_id() << "\n" << "  Processing time: " << worker.get_processing_duration()
                   << "\n";
                os << "  Queue type: ";
                if (worker.get_queue()->get_queue_type() == LIFO)
                {
                    os << "LIFO" << "\n";
                }
                else
                {
                    os << "FIFO" << "\n";
                }
                os << "  Receivers:" << "\n";
                auto sorted_receivers = sort_map(worker.receiver_preferences_);
                for (auto &receiver: sorted_receivers)
                {
                    if (receiver.first->get_receiver_type() == WORKER)
                    {
                        os << "    worker #" << receiver.first->get_id() << "\n";
                    }
                    else
                    {
                        os << "    storehouse #" << receiver.first->get_id() << "\n";
                    }
                }
                os << "\n";
                ++it_w;
                break;
            }
        }
        ++id;
    }
    os << "\n" << "== STOREHOUSES ==" << "\n" << "\n";
    auto it_s = f.storehouse_cbegin();
    id = 1;
    while (it_s != f.storehouse_cend())
    {
        for(auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); it++)
        {
            const auto &store = *it;
            if (id == store.get_id())
            {
                os << "STOREHOUSE #" << store.get_id() << "\n" << "\n";
                ++it_s;
                break;
            }
        }
        ++id;
    }
}

void generate_simulation_turn_report(const Factory &f, std::ostream &os, Time t)
{
    os << "=== [ Turn: " << t << " ] ===" << "\n" << "\n";
    os << "== WORKERS ==" << "\n";
    for (auto it = f.worker_cbegin(); it != f.worker_cend(); ++it)
    {
        const auto &worker = *(it);
        os << "\nWORKER #" << worker.get_id() << "\n";
        os << "  PBuffer: ";
        if (worker.get_processing_buffer().has_value())
        {
            os << "#" << worker.get_processing_buffer().value().get_id() << " (pt = " << t - worker.get_package_processing_start_time()<< ")\n";
        }
        else
        {
            os << "(empty)\n";
        }
        os << "  Queue: ";
        if (!worker.get_queue()->empty())
        {
            for (auto it2 = worker.get_queue()->cbegin(); it2 != worker.get_queue()->cend(); ++it2)
            {
                os << "#" << it2->get_id();
                if (std::next(it2) != worker.get_queue()->cend())
                {
                    os << ", ";
                }
            }
            os << "\n";
        }
        else
        {
            os << "(empty)\n";
        }
        os << "  SBuffer: ";
        if (worker.get_sending_buffer().has_value())
        {
            os << "#" << worker.get_sending_buffer().value().get_id() << "\n";
        }
        else
        {
            os << "(empty)\n";
        }
    }
    os << "\n\n== STOREHOUSES ==" << "\n";
    for (auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it)
    {
        const auto &storehouse = *(it);
        os << "\nSTOREHOUSE #" << storehouse.get_id() << "\n";
        os << "  Stock: ";
        if (storehouse.cbegin() != storehouse.cend())
        {
            for (auto it2 = storehouse.cbegin(); it2 != storehouse.cend(); ++it2)
            {
                os << "#" << it2->get_id();
                if (std::next(it2) != storehouse.cend()) os << ", ";
            }
            os << "\n";
        }
        else os << "(empty)\n";
        os << "\n";
    }
}

std::vector<std::pair<IPackageReceiver*, double>> sort_map(const ReceiverPreferences &receiver_preferences)
{
    typedef std::pair<IPackageReceiver*, double> pair;
    std::vector<pair> sorted_receivers;
    std::copy(receiver_preferences.begin(),
              receiver_preferences.end(),
              std::back_inserter<std::vector<pair>>(sorted_receivers));

    std::sort(sorted_receivers.begin(), sorted_receivers.end(),
              [](const pair &l, const pair &r) {
                  return l.first->get_id() < r.first->get_id();
              });
    std::sort(sorted_receivers.begin(), sorted_receivers.end(),
              [](const pair &l, const pair &r) {
                  return l.first->get_receiver_type() < r.first->get_receiver_type();
              });
    return sorted_receivers;
}