//
// Created by mcopik on 4/22/18.
//

#include <iostream>
#include <tuple>
#include <utility>
#include <cmath>

#include <util/print_tuple.hpp>

// Simple loops at the same level
std::tuple<int, int, int> simple_loop(int len)
{
    int iter_counter = 0;
    double sum = 0.0;
    // Simple loop, constant length, simple update
    for(int i = 0; i < len; ++i) {
        sum += 1.5 * i;
        ++iter_counter;
    }
    int iter_counter_second = 0;
    // Simple loop, const length, complex update
    for(int i = 0; i < len; i += 2*i) {
        sum += 2.5 * i;
        ++iter_counter_second;
    }
    int iter_counter_third = 0;
    // Simple loop, decrease
    for(int i = len*2; i >= 0; --i) {
        sum += i;
        ++iter_counter_third;
    }
    return std::make_tuple(iter_counter, iter_counter_second, iter_counter_third);
}

// Simple nested loop
//std::tuple<int, int, int> simple_nested_loop(int len)
//{
//    int iter_counter = 0;
//    double sum = 0.0;
//    // Nested, simple
//    for(int i = 0; i < len; ++i) {
//        for(int j = 0; j < len/2; ++j) {
//            sum = sum + std::pow(sum, 2.0)*0.1;
//            ++iter_counter;
//        }
//    }
//    int iter_counter_second = 0;
//    // Nested, complex update
//    for(int i = len/3; i >= 15; --i) {
//        for(int j = 0; j < len/2; j += 1.5*j + 3) {
//            sum = sum + std::pow(sum, 2.0)*0.1;
//            ++iter_counter_second;
//        }
//    }
//    int iter_counter_third = 0;
//    // Nested triple
//    for(int i = 10; i < len; ++i) {
//        for(int j = 0; j < len; ++j) {
//            for(int k = len; k >= 10; --k) {
//                sum = sum + std::pow(sum, 2.0) * 0.1;
//                ++iter_counter_third;
//            }
//        }
//    }
//    return std::make_tuple(iter_counter, iter_counter_second, iter_counter_third);
//}
//
////Nested multipath loop
//std::tuple<int, int, int> nested_multipath_loop(int len)
//{
//    int iter_nested = 0, iter_nested_second = 0, iter_nested_third = 0;
//    double sum = 0.0;
//    // Multipath on first and second level
//    for(int i = 0; i < 10; ++i) {
//
//        for(int j = 0; j < len/2; j = j + 2) {
//            sum -= 1.0;
//            ++iter_nested;
//        }
//
//        for(int j = 0; j < 20; ++j) {
//
//            for(int k = len; k > 5; k = k / 2) {
//                sum = sum + std::pow(sum, 2.0)*0.1;
//                ++iter_nested_second;
//            }
//
//            for(int k = 1; k < len; k *= 3) {
//                sum += 1.5;
//                ++iter_nested_third;
//            }
//        }
//    }
//
//    return std::make_tuple(iter_nested, iter_nested_second, iter_nested_third);
//}

int main(int argc, char ** argv)
{
    int len = 100;
    auto res = simple_loop(len);
//    auto res2 = simple_nested_loop(len);
//    auto res3 = nested_multipath_loop(len);
    std::cout << "Iteration counters:" << '\n';
    std::cout << "Simple: " << res << '\n';
//    std::cout << "Simple, nested: " << res2 << '\n';
//    std::cout << "Multipath, nested: " << res3 << '\n';

    return 0;
}