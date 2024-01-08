//
// Created by Piotr on 20.12.2023.
//

#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP

#include "config.hpp"
#include "types.hpp"
#include "storage_types.hpp"
#include "nodes.hpp"
#include <iostream>
#include <list>
#include <sstream>


// uniwersalny dla węzłów szablon klasy Node
template <class Node>
class NodeCollection
{
    // Typ danych przechowujący listę węzłów.
    using node_list = typename std::list<Node>;

public:
    // Typ iteratorów dla iteracji po węzłach.
    using iterator = typename node_list::iterator;
    using const_iterator = typename node_list::const_iterator;
    // Dodawanie węzła do kolekcji.
    void add(Node&& node) { my_nodes.push_back(std::move(node)); }
    // Wyszukiwanie węzła o określonym identyfikatorze.
    iterator find_by_id(ElementID id)
    {
        for (auto iter = my_nodes.begin(); iter != my_nodes.end(); iter++)
        {
            if (iter->get_id() == id)
            {
                return iter;
            }
        }
        return my_nodes.end();
    }
    // Wyszukiwanie węzła o określonym identyfikatorze (wersja const).
    const_iterator find_by_id(ElementID id) const
    {
        for (auto iter = my_nodes.cbegin(); iter != my_nodes.cend(); iter++)
        {
            if (iter->get_id() == id)
            {
                return iter;
            }
        }
        return my_nodes.cend();
    }
    // Usuwanie węzła o określonym identyfikatorze.
    void remove_by_id(ElementID id)
    {
        auto iter = find_by_id(id);
        if (iter != my_nodes.end())
        {
            my_nodes.erase(iter);
        }
    }
    // iteratory
    const_iterator begin() const { return my_nodes.cbegin(); }
    const_iterator end() const { return my_nodes.cend(); }
    const_iterator cbegin() const { return my_nodes.cbegin(); }
    const_iterator cend() const { return my_nodes.cend(); }
    iterator begin() { return my_nodes.begin(); }
    iterator end() { return my_nodes.end(); }
private:
    // Kontener przechowujący węzły.
    node_list my_nodes;
};

enum kolor_wezla_enum
{
    NIEODWIEDZONY, ODWIEDZONY, ZWERYFIKOWANY
};

using kolor_typ = std::map<PackageSender*, kolor_wezla_enum>;

class Factory
{
private:
    // Kolekcje węzłów różnych typów w fabryce.
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;

    // Prywatna funkcja pomocnicza do usuwania pracowników i magazynów z kolekcji węzłów.
    void remove_receiver(NodeCollection<Storehouse>& collection, ElementID id);
    void remove_receiver(NodeCollection<Worker>& collection, ElementID id);

public:
    // Dodawanie magazynu do kolekcji magazynów.
    void add_storehouse(Storehouse&& s) { storehouses_.add(std::move(s)); };
    // Usuwanie magazynu z kolekcji magazynów.
    void remove_storehouse(ElementID id) { remove_receiver(storehouses_, id); };

    // iteratory magazynów
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); };
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); };
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return storehouses_.cbegin(); };
    NodeCollection<Storehouse>::const_iterator storehouse_cend () const { return storehouses_.cend(); };
    NodeCollection<Storehouse>::iterator storehouse_begin() { return storehouses_.begin(); };
    NodeCollection<Storehouse>::iterator storehouse_end () { return storehouses_.end(); };

    // Dodawanie pracownika do kolekcji pracowników.
    void add_worker(Worker&& w) { workers_.add(std::move(w)); };
    // Usuwanie pracownika z kolekcji pracowników.
    void remove_worker(ElementID id) { remove_receiver(workers_, id); };

    // iteratory pracowników
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return workers_.find_by_id(id); };
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); };
    NodeCollection<Worker>::const_iterator worker_cbegin() const { return workers_.cbegin();} ;
    NodeCollection<Worker>::const_iterator worker_cend () const { return workers_.cend(); };
    NodeCollection<Worker>::iterator worker_begin() { return workers_.begin(); };
    NodeCollection<Worker>::iterator worker_end () { return workers_.end(); };

    // Dodawanie rampy do kolekcji ramp.
    void add_ramp(Ramp&& r) { ramps_.add(std::move(r)); };

    // Usuwanie rampy z kolekcji ramp.
    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); };

    // iteratory ramp
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); };
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); };
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return ramps_.cbegin(); };
    NodeCollection<Ramp>::const_iterator ramp_cend () const { return ramps_.cend(); };
    NodeCollection<Ramp>::iterator ramp_begin() { return ramps_.begin(); };
    NodeCollection<Ramp>::iterator ramp_end () { return ramps_.end(); };

    // Sprawdzenie spójności fabryki.
    bool is_consistent();
    // Wykonanie dostaw
    void do_deliveries(Time t);
    // Wykonywanie pracy przez pracowników
    void do_work(Time t);
    // Przekazywanie paczek między węzłami
    void do_package_passing();
};

enum class ElementType
{
    RAMP,
    WORKER,
    LINK,
    STOREHOUSE
};

struct ParsedLineData
{
    ElementType element_type;
    std::map<std::string, std::string> parameters;
};

// wczytywanie warunków początkowych fabryki
Factory load_factory_structure(std::istream &is);

void save_factory_structure(Factory &factory, std::ostream &os);

ParsedLineData parse_line(std::string line);


#endif //NETSIM_FACTORY_HPP
