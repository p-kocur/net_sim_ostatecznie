//
// Created by pawel on 10.12.2023.
//

#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <functional>

using size_type = std::size_t;
using ElementID = unsigned int; // zwykła liczba całkowita

using Time = unsigned int; // reprezentacja czasu
using TimeOffset = unsigned int; // reprezentacja długości czasu

using ProbabilityGenerator = std::function<double()>;

#endif //TYPES_HPP_
