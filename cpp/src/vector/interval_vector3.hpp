#pragma once

template<IntervalType Interval>
struct IntervalVector3 {
private:
    Interval x_;
    Interval y_;
    Interval z_;

public:
    explicit IntervalVector3() = default;

    ~IntervalVector3() = default;

    IntervalVector3(const IntervalVector3& interval_vector3) = default;

    IntervalVector3& operator=(const IntervalVector3& interval_vector3) = default;

    IntervalVector3(IntervalVector3&& interval_vector3) = default;

    IntervalVector3& operator=(IntervalVector3&& interval_vector3) = default;

    explicit IntervalVector3(const Interval& x, const Interval& y, const Interval& z) : x_(x), y_(y), z_(z) {}

    Interval& x() {
        return x_;
    }

    Interval& y() {
        return y_;
    }

    Interval& z() {
        return z_;
    }

    const Interval& x() const {
        return x_;
    }

    const Interval& y() const {
        return y_;
    }

    const Interval& z() const {
        return z_;
    }

    IntervalVector3 operator+() const {
        return IntervalVector3(+x_, +y_, +z_);
    }

    IntervalVector3 operator-() const {
        return IntervalVector3(-x_, -y_, -z_);
    }

    IntervalVector3 operator+(const IntervalVector3& interval_vector3) const {
        return IntervalVector3(x_ + interval_vector3.x_, y_ + interval_vector3.y_, z_ + interval_vector3.z_);
    }

    IntervalVector3 operator-(const IntervalVector3& interval_vector3) const {
        return IntervalVector3(x_ - interval_vector3.x_, y_ - interval_vector3.y_, z_ - interval_vector3.z_);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr() + z_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const IntervalVector3& interval_vector3) {
        return ostream << "(" << interval_vector3.x_ << " | " << interval_vector3.y_ << " | " << interval_vector3.z_ << ")";
    }

    IntervalVector2<Interval> project(const Interval& phi, const Interval& theta) const {
        const Interval& cos_phi = phi.cos();
        const Interval& sin_phi = phi.sin();
        const Interval& cos_theta = theta.cos();
        const Interval& sin_theta = theta.sin();
        return IntervalVector2<Interval>(
            -x_ * sin_phi + y_ * cos_phi,
            (x_ * cos_phi + y_ * sin_phi) * cos_theta - z_ * sin_theta
        );
    }

    std::vector<IntervalVector2<Interval>> project_hull(const Interval& phi, const Interval& theta) const {
        const Interval cos_phi = phi.cos();
        const Interval sin_phi = phi.sin();
        const Interval cos_theta = theta.cos();
        const Interval sin_theta = theta.sin();
        const IntervalVector2<Interval> projection = IntervalVector2<Interval>(
            -x_ * sin_phi + y_ * cos_phi,
            (x_ * cos_phi + y_ * sin_phi) * cos_theta - z_ * sin_theta
        );
        return std::vector<IntervalVector2<Interval>>{
            IntervalVector2<Interval>(Interval(projection.x().min()), Interval(projection.y().min())),
            IntervalVector2<Interval>(Interval(projection.x().min()), Interval(projection.y().max())),
            IntervalVector2<Interval>(Interval(projection.x().max()), Interval(projection.y().min())),
            IntervalVector2<Interval>(Interval(projection.x().max()), Interval(projection.y().max()))
        };
    }
};
