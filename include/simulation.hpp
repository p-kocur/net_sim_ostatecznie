
#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include <reports.hpp>
#include <factory.hpp>

void simulate(Factory &f, TimeOffset d, std::function<void (Factory&, Time)>);

#endif //SIMULATION_HPP
