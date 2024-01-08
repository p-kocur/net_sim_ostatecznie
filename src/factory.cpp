
#include <iostream>
#include "factory.hpp"


bool czy_nadawca_posiada_osiagalny_magazyn(const PackageSender *nadawca, std::map<const PackageSender*, kolor_wezla_enum> &kolor)
{
    if (kolor[nadawca] == kolor_wezla_enum::ZWERYFIKOWANY)
    {
        return true;
    }
    kolor[nadawca] = kolor_wezla_enum::ODWIEDZONY;
    if ((*nadawca).receiver_preferences_.cend() == (*nadawca).receiver_preferences_.cbegin())
    {
        throw std::logic_error("Nadawca nie ma ani jednego odbiorcy");
    }
    bool czy_nadawca_ma_choc_jednego_odbiorce_innego_niz_siebie_samego = false;
    for (auto &[odbiorca, probability]: nadawca->receiver_preferences_.get_preferences())
    {
        if (odbiorca->get_receiver_type() == ReceiverType::STOREHOUSE)
        {
            czy_nadawca_ma_choc_jednego_odbiorce_innego_niz_siebie_samego = true;
        }
        else if (odbiorca->get_receiver_type() == ReceiverType::WORKER)
        {
            IPackageReceiver* receiver_ptr = odbiorca;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendercv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if (sendercv_ptr == nadawca)
            {
                continue;
            }
            czy_nadawca_ma_choc_jednego_odbiorce_innego_niz_siebie_samego = true;
            if (kolor.find(sendercv_ptr)->second == kolor_wezla_enum::NIEODWIEDZONY)
            {
                czy_nadawca_posiada_osiagalny_magazyn(sendercv_ptr, kolor);
            }
        }
    }
    kolor[nadawca] = kolor_wezla_enum::ZWERYFIKOWANY;
    if (czy_nadawca_ma_choc_jednego_odbiorce_innego_niz_siebie_samego)
    {
        return true;
    }
    else
    {
        throw std::logic_error("Nie ma magazynu");
    }
}

bool Factory::is_consistent()
{
    std::map<const PackageSender*, kolor_wezla_enum> kolor_typ;
    for (auto &it : ramps_)
    {
        Ramp *ramp = &it;
        auto wezel = dynamic_cast<PackageSender*>(ramp);
        kolor_typ[wezel] = kolor_wezla_enum::NIEODWIEDZONY;
    }
    for (auto &it : workers_)
    {
        Worker* worker = &it;
        auto wezel = dynamic_cast<PackageSender*>(worker);
        kolor_typ[wezel] = kolor_wezla_enum::NIEODWIEDZONY;
    }
    try
    {
        for(auto &it : ramps_)
        {
            Ramp* ramp = &it;
            auto rampa = dynamic_cast<PackageSender*>(ramp);
            czy_nadawca_posiada_osiagalny_magazyn(rampa, kolor_typ);
        }
    }
    catch (std::logic_error &)
    {
        return false;
    }
    return true;
}

void Factory::do_work(Time t)
{
    for (auto& worker : workers_)
    {
        worker.do_work(t);
    }
}

void Factory::do_deliveries(Time t)
{
    for (auto& ramp : ramps_)
    {
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing()
{
    for (auto& ramp : ramps_)
    {
        ramp.send_package();
    }
    for (auto& worker : workers_)
    {
        worker.send_package();
    }
}

void Factory::remove_receiver(NodeCollection<Storehouse>& collection, ElementID id)
{
    for(auto& ramp : ramps_)
    {
        ramp.receiver_preferences_.remove_receiver(&*collection.find_by_id(id));
    }
    for (auto& worker : workers_)
    {
        worker.receiver_preferences_.remove_receiver(&*collection.find_by_id(id));
    }
    storehouses_.remove_by_id(id);
}

void Factory::remove_receiver(NodeCollection<Worker>& collection, ElementID id)
{
    for(auto& ramp : ramps_)
    {
        ramp.receiver_preferences_.remove_receiver(&*collection.find_by_id(id));
    }
    for(auto &worker : workers_)
    {
        worker.receiver_preferences_.remove_receiver(&*collection.find_by_id(id));
    }
    workers_.remove_by_id(id);
}

ParsedLineData parse_line(std::string line)
{
    std::vector<std::string> tokens;
    std::string token;
    ParsedLineData parsed_line;
    std::stringstream token_stream(line);

    while (std::getline(token_stream, token, ' '))
    {
        tokens.push_back(token);
    }

    if (tokens[0] == "LOADING_RAMP")
    {
        parsed_line.element_type = ElementType::RAMP;
    }
    else if (tokens[0] == "WORKER")
    {
        parsed_line.element_type = ElementType::WORKER;
    }
    else if (tokens[0] == "STOREHOUSE")
    {
        parsed_line.element_type = ElementType::STOREHOUSE;
    }
    else if (tokens[0] == "LINK")
    {
        parsed_line.element_type = ElementType::LINK;
    }
    else
    {
        throw std::logic_error("Napotkano nieprawidłowy typ elementu.");
    }
    tokens.erase(tokens.begin());

    for (auto element: tokens)
    {
        std::stringstream values_stream(element);
        std::string key;
        std::string value;
        int i = 1;
        while (std::getline(values_stream, element, '='))
        {
            if (i == 1)
            {
                key = element;
            }
            if (i == 2)
            {
                value = element;
            }
            else
            {
                std::logic_error("Nieprawidłowy format tokenu.");
            }
            i++;
        }
        parsed_line.parameters[key] = value;
    }

    return parsed_line;
}


void save_factory_structure(Factory &factory, std::ostream &os)
{
    os << "; == LOADING RAMPS ==\n\n";
    for (auto ramp = factory.ramp_cbegin(); ramp != factory.ramp_cend(); ramp++)
    {
        os << "LOADING_RAMP id=" + std::to_string((*ramp).get_id()) + " delivery-interval=" + std::to_string((*ramp).get_delivery_interval()) + "\n";
    }
    os << "; == WORKERS ==\n\n";
    for (auto worker = factory.worker_cbegin(); worker != factory.worker_cend(); worker++)
    {
        if ((*worker).get_queue()->get_queue_type() == PackageQueueType::FIFO)
        {
            os << "WORKER id=" + std::to_string((*worker).get_id()) + " processing-time=" + std::to_string((*worker).get_processing_duration()) + " queue-type=FIFO" + "\n";
        }
        if ((*worker).get_queue()->get_queue_type() == PackageQueueType::LIFO)
        {
            os << "WORKER id=" + std::to_string((*worker).get_id()) + " processing-time=" + std::to_string((*worker).get_processing_duration()) + " queue-type=LIFO" + "\n";
        }
    }

    os << "; == STOREHOUSES ==\n\n";
    for (auto storehouse = factory.storehouse_cbegin(); storehouse != factory.storehouse_cend(); storehouse++)
    {
        os << "STOREHOUSE id=" + std::to_string((*storehouse).get_id()) + "\n";
    }

    os << "; == LINKS ==\n\n";

    for (auto ramp = factory.ramp_cbegin(); ramp != factory.ramp_cend(); ramp++)
    {
        for (auto receiver = (*ramp).receiver_preferences_.cbegin(); receiver != (*ramp).receiver_preferences_.cend(); receiver++)
        {
            if((*receiver).first->get_receiver_type() == ReceiverType::WORKER)
            {
                os << "LINK src=ramp-" + std::to_string((*ramp).get_id()) + " dest=worker-" + std::to_string((*receiver).first->get_id()) + "\n";
            }
        }
    }

    for (auto worker = factory.worker_cbegin(); worker != factory.worker_cend(); worker++)
    {
        for (auto receiver = (*worker).receiver_preferences_.cbegin(); receiver != (*worker).receiver_preferences_.cend(); receiver++)
        {
            if((*receiver).first->get_receiver_type() == ReceiverType::STOREHOUSE)
            {
                os << "LINK src=worker-" + std::to_string((*worker).get_id()) + " dest=store-" + std::to_string((*receiver).first->get_id()) + "\n";
            }
            if((*receiver).first->get_receiver_type() == ReceiverType::WORKER)
            {
                os << "LINK src=worker-" + std::to_string((*worker).get_id()) + " dest=worker-" + std::to_string((*receiver).first->get_id()) + "\n";
            }
        }
    }
}

Factory load_factory_structure(std::istream &is)
{
    Factory factory;
    std::string line;

    while (std::getline(is, line))
    {
        if (!line.empty() && line[0] != ';')
        {
            ParsedLineData parsed_line = parse_line(line);

            if (parsed_line.element_type == ElementType::RAMP)
            {
                factory.add_ramp(Ramp(static_cast<ElementID>(std::stoi(parsed_line.parameters["id"])),
                                      static_cast<TimeOffset>(std::stoi(parsed_line.parameters["delivery-interval"]))));
            }
            else if (parsed_line.element_type == ElementType::WORKER)
            {
                PackageQueueType queue_type;
                if (parsed_line.parameters["queue-type"] == "LIFO")
                {
                    queue_type = PackageQueueType::LIFO;
                }
                else
                {
                    queue_type = PackageQueueType::FIFO;
                }
                factory.add_worker(Worker(static_cast<ElementID>(std::stoi(parsed_line.parameters["id"])),
                                          static_cast<TimeOffset>(std::stoi(parsed_line.parameters["processing-time"])), std::make_unique<PackageQueue>(queue_type)));
            }
            else if (parsed_line.element_type == ElementType::STOREHOUSE)
            {
                factory.add_storehouse(Storehouse(static_cast<ElementID>(std::stoi(parsed_line.parameters["id"]))));
            }
            else if (parsed_line.element_type == ElementType::LINK)
            {
                std::stringstream src_stream(parsed_line.parameters["src"]);
                std::stringstream dest_stream(parsed_line.parameters["dest"]);
                std::string src_key, src_value, dest_key, dest_value, token;

                std::getline(src_stream, src_key, '-');
                std::getline(src_stream, src_value, '-');

                std::getline(dest_stream, dest_key, '-');
                std::getline(dest_stream, dest_value, '-');

                if (src_key=="ramp" && dest_key=="worker")
                {
                    Ramp& r = *(factory.find_ramp_by_id(static_cast<ElementID>(std::stoi(src_value))));
                    r.receiver_preferences_.add_receiver
                            (&(*factory.find_worker_by_id(static_cast<ElementID>(std::stoi(dest_value)))));
                }
                else if(src_key=="worker" && dest_key=="worker")
                {
                    Worker& w = *(factory.find_worker_by_id(static_cast<ElementID>(std::stoi(src_value))));
                    w.receiver_preferences_.add_receiver
                            (&(*factory.find_worker_by_id(static_cast<ElementID>(std::stoi(dest_value)))));
                }
                else if(src_key=="worker" && dest_key=="store")
                {
                    Worker& w = *(factory.find_worker_by_id(static_cast<ElementID>(std::stoi(src_value))));
                    w.receiver_preferences_.add_receiver
                            (&(*factory.find_storehouse_by_id(static_cast<ElementID>(std::stoi(dest_value)))));
                }
                else if(src_key=="ramp" && dest_key=="store")
                {
                    Ramp& r = *(factory.find_ramp_by_id(static_cast<ElementID>(std::stoi(src_value))));
                    r.receiver_preferences_.add_receiver
                            (&(*factory.find_storehouse_by_id(static_cast<ElementID>(std::stoi(dest_value)))));
                }
            }
        }
    }
    return factory;
}

