#pragma once

#include "vector/vector_type.hpp"

#include "vector/vector2.hpp"
static_assert(Vector2Type<Vector2<FloatInterval>, FloatInterval>);
static_assert(Vector2Type<Vector2<BoostInterval>, BoostInterval>);
static_assert(Vector2Type<Vector2<MpfiInterval>, MpfiInterval>);

#include "vector/vector3.hpp"
static_assert(Vector3Type<Vector3<FloatInterval>, FloatInterval>);
static_assert(Vector3Type<Vector3<BoostInterval>, BoostInterval>);
static_assert(Vector3Type<Vector3<MpfiInterval>, MpfiInterval>);
