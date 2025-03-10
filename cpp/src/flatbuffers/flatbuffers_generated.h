// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_FLATBUFFERS_FLATBUFFERS_H_
#define FLATBUFFERS_GENERATED_FLATBUFFERS_FLATBUFFERS_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 2 &&
              FLATBUFFERS_VERSION_MINOR == 0 &&
              FLATBUFFERS_VERSION_REVISION == 8,
             "Non-compatible flatbuffers version included");

namespace FlatBuffers {

struct Vector2;

struct Vertex;

struct Edge;

struct Polygon;
struct PolygonBuilder;

struct Polyhedron;
struct PolyhedronBuilder;

struct Interval;

struct Id;

struct Projection;
struct ProjectionBuilder;

struct Box2;
struct Box2Builder;

struct Box3;
struct Box3Builder;

struct Cover;
struct CoverBuilder;

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) Vector2 FLATBUFFERS_FINAL_CLASS {
 private:
  double x_;
  double y_;

 public:
  Vector2()
      : x_(0),
        y_(0) {
  }
  Vector2(double _x, double _y)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)) {
  }
  double x() const {
    return flatbuffers::EndianScalar(x_);
  }
  double y() const {
    return flatbuffers::EndianScalar(y_);
  }
};
FLATBUFFERS_STRUCT_END(Vector2, 16);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) Vertex FLATBUFFERS_FINAL_CLASS {
 private:
  double x_;
  double y_;
  double z_;

 public:
  Vertex()
      : x_(0),
        y_(0),
        z_(0) {
  }
  Vertex(double _x, double _y, double _z)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)),
        z_(flatbuffers::EndianScalar(_z)) {
  }
  double x() const {
    return flatbuffers::EndianScalar(x_);
  }
  double y() const {
    return flatbuffers::EndianScalar(y_);
  }
  double z() const {
    return flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(Vertex, 24);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) Edge FLATBUFFERS_FINAL_CLASS {
 private:
  FlatBuffers::Vector2 from_;
  FlatBuffers::Vector2 to_;

 public:
  Edge()
      : from_(),
        to_() {
  }
  Edge(const FlatBuffers::Vector2 &_from, const FlatBuffers::Vector2 &_to)
      : from_(_from),
        to_(_to) {
  }
  const FlatBuffers::Vector2 &from() const {
    return from_;
  }
  const FlatBuffers::Vector2 &to() const {
    return to_;
  }
};
FLATBUFFERS_STRUCT_END(Edge, 32);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) Interval FLATBUFFERS_FINAL_CLASS {
 private:
  double min_;
  double max_;

 public:
  Interval()
      : min_(0),
        max_(0) {
  }
  Interval(double _min, double _max)
      : min_(flatbuffers::EndianScalar(_min)),
        max_(flatbuffers::EndianScalar(_max)) {
  }
  double min() const {
    return flatbuffers::EndianScalar(min_);
  }
  double max() const {
    return flatbuffers::EndianScalar(max_);
  }
};
FLATBUFFERS_STRUCT_END(Interval, 16);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) Id FLATBUFFERS_FINAL_CLASS {
 private:
  uint64_t bits_;
  uint8_t depth_;
  int8_t padding0__;  int16_t padding1__;  int32_t padding2__;
  FlatBuffers::Interval interval_;

 public:
  Id()
      : bits_(0),
        depth_(0),
        padding0__(0),
        padding1__(0),
        padding2__(0),
        interval_() {
    (void)padding0__;
    (void)padding1__;
    (void)padding2__;
  }
  Id(uint64_t _bits, uint8_t _depth, const FlatBuffers::Interval &_interval)
      : bits_(flatbuffers::EndianScalar(_bits)),
        depth_(flatbuffers::EndianScalar(_depth)),
        padding0__(0),
        padding1__(0),
        padding2__(0),
        interval_(_interval) {
    (void)padding0__;
    (void)padding1__;
    (void)padding2__;
  }
  uint64_t bits() const {
    return flatbuffers::EndianScalar(bits_);
  }
  uint8_t depth() const {
    return flatbuffers::EndianScalar(depth_);
  }
  const FlatBuffers::Interval &interval() const {
    return interval_;
  }
};
FLATBUFFERS_STRUCT_END(Id, 32);

struct Polygon FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PolygonBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_EDGES = 4
  };
  const flatbuffers::Vector<const FlatBuffers::Edge *> *edges() const {
    return GetPointer<const flatbuffers::Vector<const FlatBuffers::Edge *> *>(VT_EDGES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_EDGES) &&
           verifier.VerifyVector(edges()) &&
           verifier.EndTable();
  }
};

struct PolygonBuilder {
  typedef Polygon Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_edges(flatbuffers::Offset<flatbuffers::Vector<const FlatBuffers::Edge *>> edges) {
    fbb_.AddOffset(Polygon::VT_EDGES, edges);
  }
  explicit PolygonBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Polygon> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Polygon>(end);
    return o;
  }
};

inline flatbuffers::Offset<Polygon> CreatePolygon(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<const FlatBuffers::Edge *>> edges = 0) {
  PolygonBuilder builder_(_fbb);
  builder_.add_edges(edges);
  return builder_.Finish();
}

inline flatbuffers::Offset<Polygon> CreatePolygonDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<FlatBuffers::Edge> *edges = nullptr) {
  auto edges__ = edges ? _fbb.CreateVectorOfStructs<FlatBuffers::Edge>(*edges) : 0;
  return FlatBuffers::CreatePolygon(
      _fbb,
      edges__);
}

struct Polyhedron FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PolyhedronBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_VERTICES = 4
  };
  const flatbuffers::Vector<const FlatBuffers::Vertex *> *vertices() const {
    return GetPointer<const flatbuffers::Vector<const FlatBuffers::Vertex *> *>(VT_VERTICES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_VERTICES) &&
           verifier.VerifyVector(vertices()) &&
           verifier.EndTable();
  }
};

struct PolyhedronBuilder {
  typedef Polyhedron Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_vertices(flatbuffers::Offset<flatbuffers::Vector<const FlatBuffers::Vertex *>> vertices) {
    fbb_.AddOffset(Polyhedron::VT_VERTICES, vertices);
  }
  explicit PolyhedronBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Polyhedron> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Polyhedron>(end);
    return o;
  }
};

inline flatbuffers::Offset<Polyhedron> CreatePolyhedron(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<const FlatBuffers::Vertex *>> vertices = 0) {
  PolyhedronBuilder builder_(_fbb);
  builder_.add_vertices(vertices);
  return builder_.Finish();
}

inline flatbuffers::Offset<Polyhedron> CreatePolyhedronDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<FlatBuffers::Vertex> *vertices = nullptr) {
  auto vertices__ = vertices ? _fbb.CreateVectorOfStructs<FlatBuffers::Vertex>(*vertices) : 0;
  return FlatBuffers::CreatePolyhedron(
      _fbb,
      vertices__);
}

struct Projection FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ProjectionBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_VECTORS = 4
  };
  const flatbuffers::Vector<const FlatBuffers::Vector2 *> *vectors() const {
    return GetPointer<const flatbuffers::Vector<const FlatBuffers::Vector2 *> *>(VT_VECTORS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_VECTORS) &&
           verifier.VerifyVector(vectors()) &&
           verifier.EndTable();
  }
};

struct ProjectionBuilder {
  typedef Projection Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_vectors(flatbuffers::Offset<flatbuffers::Vector<const FlatBuffers::Vector2 *>> vectors) {
    fbb_.AddOffset(Projection::VT_VECTORS, vectors);
  }
  explicit ProjectionBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Projection> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Projection>(end);
    return o;
  }
};

inline flatbuffers::Offset<Projection> CreateProjection(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<const FlatBuffers::Vector2 *>> vectors = 0) {
  ProjectionBuilder builder_(_fbb);
  builder_.add_vectors(vectors);
  return builder_.Finish();
}

inline flatbuffers::Offset<Projection> CreateProjectionDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<FlatBuffers::Vector2> *vectors = nullptr) {
  auto vectors__ = vectors ? _fbb.CreateVectorOfStructs<FlatBuffers::Vector2>(*vectors) : 0;
  return FlatBuffers::CreateProjection(
      _fbb,
      vectors__);
}

struct Box2 FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef Box2Builder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_THETA = 4,
    VT_PHI = 6,
    VT_PROJECTIONS = 8,
    VT_OUT = 10
  };
  const FlatBuffers::Id *theta() const {
    return GetStruct<const FlatBuffers::Id *>(VT_THETA);
  }
  const FlatBuffers::Id *phi() const {
    return GetStruct<const FlatBuffers::Id *>(VT_PHI);
  }
  const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>> *projections() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>> *>(VT_PROJECTIONS);
  }
  const flatbuffers::Vector<uint8_t> *out() const {
    return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_OUT);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<FlatBuffers::Id>(verifier, VT_THETA, 8) &&
           VerifyField<FlatBuffers::Id>(verifier, VT_PHI, 8) &&
           VerifyOffset(verifier, VT_PROJECTIONS) &&
           verifier.VerifyVector(projections()) &&
           verifier.VerifyVectorOfTables(projections()) &&
           VerifyOffset(verifier, VT_OUT) &&
           verifier.VerifyVector(out()) &&
           verifier.EndTable();
  }
};

struct Box2Builder {
  typedef Box2 Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_theta(const FlatBuffers::Id *theta) {
    fbb_.AddStruct(Box2::VT_THETA, theta);
  }
  void add_phi(const FlatBuffers::Id *phi) {
    fbb_.AddStruct(Box2::VT_PHI, phi);
  }
  void add_projections(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>>> projections) {
    fbb_.AddOffset(Box2::VT_PROJECTIONS, projections);
  }
  void add_out(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> out) {
    fbb_.AddOffset(Box2::VT_OUT, out);
  }
  explicit Box2Builder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Box2> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Box2>(end);
    return o;
  }
};

inline flatbuffers::Offset<Box2> CreateBox2(
    flatbuffers::FlatBufferBuilder &_fbb,
    const FlatBuffers::Id *theta = nullptr,
    const FlatBuffers::Id *phi = nullptr,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>>> projections = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> out = 0) {
  Box2Builder builder_(_fbb);
  builder_.add_out(out);
  builder_.add_projections(projections);
  builder_.add_phi(phi);
  builder_.add_theta(theta);
  return builder_.Finish();
}

inline flatbuffers::Offset<Box2> CreateBox2Direct(
    flatbuffers::FlatBufferBuilder &_fbb,
    const FlatBuffers::Id *theta = nullptr,
    const FlatBuffers::Id *phi = nullptr,
    const std::vector<flatbuffers::Offset<FlatBuffers::Projection>> *projections = nullptr,
    const std::vector<uint8_t> *out = nullptr) {
  auto projections__ = projections ? _fbb.CreateVector<flatbuffers::Offset<FlatBuffers::Projection>>(*projections) : 0;
  auto out__ = out ? _fbb.CreateVector<uint8_t>(*out) : 0;
  return FlatBuffers::CreateBox2(
      _fbb,
      theta,
      phi,
      projections__,
      out__);
}

struct Box3 FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef Box3Builder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_THETA = 4,
    VT_PHI = 6,
    VT_ALPHA = 8,
    VT_PROJECTIONS = 10,
    VT_PROJECTION = 12,
    VT_BOX2S = 14,
    VT_COMPLETE = 16,
    VT_IN = 18
  };
  const FlatBuffers::Id *theta() const {
    return GetStruct<const FlatBuffers::Id *>(VT_THETA);
  }
  const FlatBuffers::Id *phi() const {
    return GetStruct<const FlatBuffers::Id *>(VT_PHI);
  }
  const FlatBuffers::Id *alpha() const {
    return GetStruct<const FlatBuffers::Id *>(VT_ALPHA);
  }
  const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>> *projections() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>> *>(VT_PROJECTIONS);
  }
  const FlatBuffers::Polygon *projection() const {
    return GetPointer<const FlatBuffers::Polygon *>(VT_PROJECTION);
  }
  const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box2>> *box2s() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box2>> *>(VT_BOX2S);
  }
  bool complete() const {
    return GetField<uint8_t>(VT_COMPLETE, 0) != 0;
  }
  int32_t in() const {
    return GetField<int32_t>(VT_IN, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<FlatBuffers::Id>(verifier, VT_THETA, 8) &&
           VerifyField<FlatBuffers::Id>(verifier, VT_PHI, 8) &&
           VerifyField<FlatBuffers::Id>(verifier, VT_ALPHA, 8) &&
           VerifyOffset(verifier, VT_PROJECTIONS) &&
           verifier.VerifyVector(projections()) &&
           verifier.VerifyVectorOfTables(projections()) &&
           VerifyOffset(verifier, VT_PROJECTION) &&
           verifier.VerifyTable(projection()) &&
           VerifyOffset(verifier, VT_BOX2S) &&
           verifier.VerifyVector(box2s()) &&
           verifier.VerifyVectorOfTables(box2s()) &&
           VerifyField<uint8_t>(verifier, VT_COMPLETE, 1) &&
           VerifyField<int32_t>(verifier, VT_IN, 4) &&
           verifier.EndTable();
  }
};

struct Box3Builder {
  typedef Box3 Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_theta(const FlatBuffers::Id *theta) {
    fbb_.AddStruct(Box3::VT_THETA, theta);
  }
  void add_phi(const FlatBuffers::Id *phi) {
    fbb_.AddStruct(Box3::VT_PHI, phi);
  }
  void add_alpha(const FlatBuffers::Id *alpha) {
    fbb_.AddStruct(Box3::VT_ALPHA, alpha);
  }
  void add_projections(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>>> projections) {
    fbb_.AddOffset(Box3::VT_PROJECTIONS, projections);
  }
  void add_projection(flatbuffers::Offset<FlatBuffers::Polygon> projection) {
    fbb_.AddOffset(Box3::VT_PROJECTION, projection);
  }
  void add_box2s(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box2>>> box2s) {
    fbb_.AddOffset(Box3::VT_BOX2S, box2s);
  }
  void add_complete(bool complete) {
    fbb_.AddElement<uint8_t>(Box3::VT_COMPLETE, static_cast<uint8_t>(complete), 0);
  }
  void add_in(int32_t in) {
    fbb_.AddElement<int32_t>(Box3::VT_IN, in, 0);
  }
  explicit Box3Builder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Box3> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Box3>(end);
    return o;
  }
};

inline flatbuffers::Offset<Box3> CreateBox3(
    flatbuffers::FlatBufferBuilder &_fbb,
    const FlatBuffers::Id *theta = nullptr,
    const FlatBuffers::Id *phi = nullptr,
    const FlatBuffers::Id *alpha = nullptr,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>>> projections = 0,
    flatbuffers::Offset<FlatBuffers::Polygon> projection = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box2>>> box2s = 0,
    bool complete = false,
    int32_t in = 0) {
  Box3Builder builder_(_fbb);
  builder_.add_in(in);
  builder_.add_box2s(box2s);
  builder_.add_projection(projection);
  builder_.add_projections(projections);
  builder_.add_alpha(alpha);
  builder_.add_phi(phi);
  builder_.add_theta(theta);
  builder_.add_complete(complete);
  return builder_.Finish();
}

inline flatbuffers::Offset<Box3> CreateBox3Direct(
    flatbuffers::FlatBufferBuilder &_fbb,
    const FlatBuffers::Id *theta = nullptr,
    const FlatBuffers::Id *phi = nullptr,
    const FlatBuffers::Id *alpha = nullptr,
    const std::vector<flatbuffers::Offset<FlatBuffers::Projection>> *projections = nullptr,
    flatbuffers::Offset<FlatBuffers::Polygon> projection = 0,
    const std::vector<flatbuffers::Offset<FlatBuffers::Box2>> *box2s = nullptr,
    bool complete = false,
    int32_t in = 0) {
  auto projections__ = projections ? _fbb.CreateVector<flatbuffers::Offset<FlatBuffers::Projection>>(*projections) : 0;
  auto box2s__ = box2s ? _fbb.CreateVector<flatbuffers::Offset<FlatBuffers::Box2>>(*box2s) : 0;
  return FlatBuffers::CreateBox3(
      _fbb,
      theta,
      phi,
      alpha,
      projections__,
      projection,
      box2s__,
      complete,
      in);
}

struct Cover FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef CoverBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_DESCRIPTION = 4,
    VT_HOLE = 6,
    VT_PLUG = 8,
    VT_BOX3S = 10
  };
  const flatbuffers::String *description() const {
    return GetPointer<const flatbuffers::String *>(VT_DESCRIPTION);
  }
  const FlatBuffers::Polyhedron *hole() const {
    return GetPointer<const FlatBuffers::Polyhedron *>(VT_HOLE);
  }
  const FlatBuffers::Polyhedron *plug() const {
    return GetPointer<const FlatBuffers::Polyhedron *>(VT_PLUG);
  }
  const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box3>> *box3s() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box3>> *>(VT_BOX3S);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_DESCRIPTION) &&
           verifier.VerifyString(description()) &&
           VerifyOffset(verifier, VT_HOLE) &&
           verifier.VerifyTable(hole()) &&
           VerifyOffset(verifier, VT_PLUG) &&
           verifier.VerifyTable(plug()) &&
           VerifyOffset(verifier, VT_BOX3S) &&
           verifier.VerifyVector(box3s()) &&
           verifier.VerifyVectorOfTables(box3s()) &&
           verifier.EndTable();
  }
};

struct CoverBuilder {
  typedef Cover Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_description(flatbuffers::Offset<flatbuffers::String> description) {
    fbb_.AddOffset(Cover::VT_DESCRIPTION, description);
  }
  void add_hole(flatbuffers::Offset<FlatBuffers::Polyhedron> hole) {
    fbb_.AddOffset(Cover::VT_HOLE, hole);
  }
  void add_plug(flatbuffers::Offset<FlatBuffers::Polyhedron> plug) {
    fbb_.AddOffset(Cover::VT_PLUG, plug);
  }
  void add_box3s(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box3>>> box3s) {
    fbb_.AddOffset(Cover::VT_BOX3S, box3s);
  }
  explicit CoverBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Cover> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Cover>(end);
    return o;
  }
};

inline flatbuffers::Offset<Cover> CreateCover(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> description = 0,
    flatbuffers::Offset<FlatBuffers::Polyhedron> hole = 0,
    flatbuffers::Offset<FlatBuffers::Polyhedron> plug = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box3>>> box3s = 0) {
  CoverBuilder builder_(_fbb);
  builder_.add_box3s(box3s);
  builder_.add_plug(plug);
  builder_.add_hole(hole);
  builder_.add_description(description);
  return builder_.Finish();
}

inline flatbuffers::Offset<Cover> CreateCoverDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *description = nullptr,
    flatbuffers::Offset<FlatBuffers::Polyhedron> hole = 0,
    flatbuffers::Offset<FlatBuffers::Polyhedron> plug = 0,
    const std::vector<flatbuffers::Offset<FlatBuffers::Box3>> *box3s = nullptr) {
  auto description__ = description ? _fbb.CreateString(description) : 0;
  auto box3s__ = box3s ? _fbb.CreateVector<flatbuffers::Offset<FlatBuffers::Box3>>(*box3s) : 0;
  return FlatBuffers::CreateCover(
      _fbb,
      description__,
      hole,
      plug,
      box3s__);
}

inline const FlatBuffers::Cover *GetCover(const void *buf) {
  return flatbuffers::GetRoot<FlatBuffers::Cover>(buf);
}

inline const FlatBuffers::Cover *GetSizePrefixedCover(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<FlatBuffers::Cover>(buf);
}

inline bool VerifyCoverBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FlatBuffers::Cover>(nullptr);
}

inline bool VerifySizePrefixedCoverBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FlatBuffers::Cover>(nullptr);
}

inline void FinishCoverBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FlatBuffers::Cover> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedCoverBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FlatBuffers::Cover> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace FlatBuffers

#endif  // FLATBUFFERS_GENERATED_FLATBUFFERS_FLATBUFFERS_H_
