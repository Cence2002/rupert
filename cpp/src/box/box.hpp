#pragma once

#include "range/range.hpp"

template<size_t Size>
struct Box {
    std::array<Range, Size> ranges;

    explicit Box(const std::array<Range, Size>& ranges) : ranges(ranges) {}

    bool operator<(const Box& box) const {
        for(size_t i = 0; i < Size; i++) {
            if(ranges.at(i) < box.ranges.at(i)) {
                return true;
            }
            if(box.ranges.at(i) < ranges.at(i)) {
                return false;
            }
        }
        return false;
    }

    Range range(size_t index) const {
        return ranges.at(index);
    }

    std::vector<Box> parts() const {
        std::array<std::pair<Range, Range>, Size> range_parts;
        for(size_t i = 0; i < Size; i++) {
            range_parts.at(i) = ranges.at(i).parts();
        }
        std::vector<Box> parts;
        for(size_t mask = 0; mask < 1ull << Size; mask++) {
            std::array<Range, Size> part;
            for(size_t i = 0; i < Size; i++) {
                if(mask & 1ull << i) {
                    part.at(i) = range_parts.at(i).first;
                } else {
                    part.at(i) = range_parts.at(i).second;
                }
            }
            parts.emplace_back(part);
        }
        return parts;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Box& box) {
        std::vector<std::string> range_strings;
        for(const Range& range: box.ranges) {
            std::stringstream stream;
            stream << range;
            range_strings.push_back(stream.str());
        }
        const std::string ranges_string = boost::algorithm::join(range_strings, ", ");
        return ostream << "<" << ranges_string << ">";
    }
};

using Box1 = Box<1>;
using Box2 = Box<2>;
using Box3 = Box<3>;
