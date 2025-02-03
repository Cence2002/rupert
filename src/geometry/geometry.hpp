#pragma once

#include "geometry/vector_2.hpp"
#include "geometry/vector_3.hpp"
#include "geometry/line.hpp"
#include "geometry/polygon.hpp"
#include "geometry/polyhedron.hpp"

static_assert(Vector2Type<Vector2<FloatInterval>, FloatInterval>);
static_assert(Vector2Type<Vector2<BoostInterval>, BoostInterval>);
static_assert(Vector2Type<Vector2<MpfiInterval>, MpfiInterval>);

static_assert(Vector3Type<Vector3<FloatInterval>, FloatInterval>);
static_assert(Vector3Type<Vector3<BoostInterval>, BoostInterval>);
static_assert(Vector3Type<Vector3<MpfiInterval>, MpfiInterval>);
