#pragma once

#include "interval/interval_type.hpp"
#include "interval/float_interval.hpp"
#include "interval/boost_interval.hpp"
#include "interval/mpfi_interval.hpp"

enum class PrintMode {
    min_and_max,
    mid_and_rad,
    mid_only,
};

inline PrintMode print_mode = PrintMode::mid_only;
inline std::streamsize print_precision = 3;

inline std::string to_string(const double value) {
    std::ostringstream stream;
    stream.precision(print_precision);
    stream << value;
    return stream.str();
}

inline std::string to_string(const double value, const std::ios_base::fmtflags format) {
    std::ostringstream stream;
    stream.precision(print_precision);
    stream.setf(format, std::ios_base::floatfield);
    stream << value;
    return stream.str();
}

template<IntervalType Interval>
std::ostream& operator<<(std::ostream& ostream, const Interval& interval) {
    switch(print_mode) {
        case PrintMode::min_and_max: {
            return ostream << to_string(interval.min().to_float(), std::ios_base::fixed) << ":" << to_string(interval.max().to_float(), std::ios_base::fixed);
        }
        case PrintMode::mid_and_rad: {
            return ostream << to_string(interval.mid().to_float()) << "~" << to_string(interval.rad().to_float(), std::ios_base::scientific);
        }
        case PrintMode::mid_only: {
            return ostream << to_string(interval.mid().to_float());
        }
        default: throw std::invalid_argument("Unknown PrintMode");
    }
}
