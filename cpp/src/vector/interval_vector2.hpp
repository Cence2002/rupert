#pragma once

template<IntervalType Interval>
struct IntervalVector2 {
private:
    Interval x_;
    Interval y_;

public:
    explicit IntervalVector2() : x_(), y_() {}

    ~IntervalVector2() = default;

    IntervalVector2(const IntervalVector2& vector2) = default;

    IntervalVector2& operator=(const IntervalVector2& vector2) = default;

    IntervalVector2(IntervalVector2&& vector2) = default;

    IntervalVector2& operator=(IntervalVector2&& vector2) = default;

    explicit IntervalVector2(const Interval& x, const Interval& y) : x_(x), y_(y) {}

    Interval& x() {
        return x_;
    }

    Interval& y() {
        return y_;
    }

    const Interval& x() const {
        return x_;
    }

    const Interval& y() const {
        return y_;
    }

    IntervalVector2 operator+() const {
        return IntervalVector2(+x_, +y_);
    }

    IntervalVector2 operator-() const {
        return IntervalVector2(-x_, -y_);
    }

    IntervalVector2 operator+(const IntervalVector2& vector2) const {
        return IntervalVector2(x_ + vector2.x_, y_ + vector2.y_);
    }

    IntervalVector2 operator-(const IntervalVector2& vector2) const {
        return IntervalVector2(x_ - vector2.x_, y_ - vector2.y_);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const IntervalVector2& vector2) {
        return ostream << "(" << vector2.x_ << " | " << vector2.y_ << ")";
    }

    IntervalVector2 rotate(const Interval& alpha) const {
        const Interval& cos_alpha = alpha.cos();
        const Interval& sin_alpha = alpha.sin();
        return IntervalVector2(
            x_ * cos_alpha - y_ * sin_alpha,
            x_ * sin_alpha + y_ * cos_alpha
        );
    }

    std::vector<IntervalVector2> rotate_hull(const Interval& alpha) const {
        const Interval cos_alpha_min = Interval(alpha.min()).cos();
        const Interval cos_alpha_mid = Interval(alpha.mid()).cos();
        const Interval cos_alpha_max = Interval(alpha.max()).cos();

        const Interval sin_alpha_min = Interval(alpha.min()).sin();
        const Interval sin_alpha_mid = Interval(alpha.mid()).sin();
        const Interval sin_alpha_max = Interval(alpha.max()).sin();

        const Interval cos_alpha_rad = Interval(alpha.rad()).cos();

        return std::vector<IntervalVector2>{
                IntervalVector2(
                    x_ * cos_alpha_min - y_ * sin_alpha_min,
                    x_ * sin_alpha_min + y_ * cos_alpha_min
                ),
                IntervalVector2(
                    x_ * cos_alpha_max - y_ * sin_alpha_max,
                    x_ * sin_alpha_max + y_ * cos_alpha_max
                ),
                IntervalVector2(
                    (x_ * cos_alpha_mid - y_ * sin_alpha_mid) / cos_alpha_rad,
                    (x_ * sin_alpha_mid + y_ * cos_alpha_mid) / cos_alpha_rad
                )
            };
    }

    Interval dot(const IntervalVector2& vector2) const {
        return x_ * vector2.x() + y_ * vector2.y();
    }

    Interval cross(const IntervalVector2& vector2) const {
        return x_ * vector2.y() - y_ * vector2.x();
    }
};
