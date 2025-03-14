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

struct Vector;

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

struct Rectangle;
struct RectangleBuilder;

struct Box;
struct BoxBuilder;

struct Boxes;
struct BoxesBuilder;

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) Vector FLATBUFFERS_FINAL_CLASS {
 private:
  double x_;
  double y_;

 public:
  Vector()
      : x_(0),
        y_(0) {
  }
  Vector(double _x, double _y)
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
FLATBUFFERS_STRUCT_END(Vector, 16);

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
  FlatBuffers::Vector from_;
  FlatBuffers::Vector to_;

 public:
  Edge()
      : from_(),
        to_() {
  }
  Edge(const FlatBuffers::Vector &_from, const FlatBuffers::Vector &_to)
      : from_(_from),
        to_(_to) {
  }
  const FlatBuffers::Vector &from() const {
    return from_;
  }
  const FlatBuffers::Vector &to() const {
    return to_;
  }
};
FLATBUFFERS_STRUCT_END(Edge, 32);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) Interval FLATBUFFERS_FINAL_CLASS {
 private:
  double min_;
  double max_;
  double mid_;
  double len_;

 public:
  Interval()
      : min_(0),
        max_(0),
        mid_(0),
        len_(0) {
  }
  Interval(double _min, double _max, double _mid, double _len)
      : min_(flatbuffers::EndianScalar(_min)),
        max_(flatbuffers::EndianScalar(_max)),
        mid_(flatbuffers::EndianScalar(_mid)),
        len_(flatbuffers::EndianScalar(_len)) {
  }
  double min() const {
    return flatbuffers::EndianScalar(min_);
  }
  double max() const {
    return flatbuffers::EndianScalar(max_);
  }
  double mid() const {
    return flatbuffers::EndianScalar(mid_);
  }
  double len() const {
    return flatbuffers::EndianScalar(len_);
  }
};
FLATBUFFERS_STRUCT_END(Interval, 32);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) Id FLATBUFFERS_FINAL_CLASS {
 private:
  uint8_t depth_;
  int8_t padding0__;
  uint16_t bits_;
  int32_t padding1__;
  FlatBuffers::Interval interval_;

 public:
  Id()
      : depth_(0),
        padding0__(0),
        bits_(0),
        padding1__(0),
        interval_() {
    (void)padding0__;
    (void)padding1__;
  }
  Id(uint8_t _depth, uint16_t _bits, const FlatBuffers::Interval &_interval)
      : depth_(flatbuffers::EndianScalar(_depth)),
        padding0__(0),
        bits_(flatbuffers::EndianScalar(_bits)),
        padding1__(0),
        interval_(_interval) {
    (void)padding0__;
    (void)padding1__;
  }
  uint8_t depth() const {
    return flatbuffers::EndianScalar(depth_);
  }
  uint16_t bits() const {
    return flatbuffers::EndianScalar(bits_);
  }
  const FlatBuffers::Interval &interval() const {
    return interval_;
  }
};
FLATBUFFERS_STRUCT_END(Id, 40);

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
  const flatbuffers::Vector<const FlatBuffers::Vector *> *vectors() const {
    return GetPointer<const flatbuffers::Vector<const FlatBuffers::Vector *> *>(VT_VECTORS);
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
  void add_vectors(flatbuffers::Offset<flatbuffers::Vector<const FlatBuffers::Vector *>> vectors) {
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
    flatbuffers::Offset<flatbuffers::Vector<const FlatBuffers::Vector *>> vectors = 0) {
  ProjectionBuilder builder_(_fbb);
  builder_.add_vectors(vectors);
  return builder_.Finish();
}

inline flatbuffers::Offset<Projection> CreateProjectionDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<FlatBuffers::Vector> *vectors = nullptr) {
  auto vectors__ = vectors ? _fbb.CreateVectorOfStructs<FlatBuffers::Vector>(*vectors) : 0;
  return FlatBuffers::CreateProjection(
      _fbb,
      vectors__);
}

struct Rectangle FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef RectangleBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_THETA = 4,
    VT_PHI = 6,
    VT_PROJECTIONS = 8,
    VT_OUT_INDICES = 10
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
  const flatbuffers::Vector<uint32_t> *out_indices() const {
    return GetPointer<const flatbuffers::Vector<uint32_t> *>(VT_OUT_INDICES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<FlatBuffers::Id>(verifier, VT_THETA, 8) &&
           VerifyField<FlatBuffers::Id>(verifier, VT_PHI, 8) &&
           VerifyOffset(verifier, VT_PROJECTIONS) &&
           verifier.VerifyVector(projections()) &&
           verifier.VerifyVectorOfTables(projections()) &&
           VerifyOffset(verifier, VT_OUT_INDICES) &&
           verifier.VerifyVector(out_indices()) &&
           verifier.EndTable();
  }
};

struct RectangleBuilder {
  typedef Rectangle Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_theta(const FlatBuffers::Id *theta) {
    fbb_.AddStruct(Rectangle::VT_THETA, theta);
  }
  void add_phi(const FlatBuffers::Id *phi) {
    fbb_.AddStruct(Rectangle::VT_PHI, phi);
  }
  void add_projections(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>>> projections) {
    fbb_.AddOffset(Rectangle::VT_PROJECTIONS, projections);
  }
  void add_out_indices(flatbuffers::Offset<flatbuffers::Vector<uint32_t>> out_indices) {
    fbb_.AddOffset(Rectangle::VT_OUT_INDICES, out_indices);
  }
  explicit RectangleBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Rectangle> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Rectangle>(end);
    return o;
  }
};

inline flatbuffers::Offset<Rectangle> CreateRectangle(
    flatbuffers::FlatBufferBuilder &_fbb,
    const FlatBuffers::Id *theta = nullptr,
    const FlatBuffers::Id *phi = nullptr,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>>> projections = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint32_t>> out_indices = 0) {
  RectangleBuilder builder_(_fbb);
  builder_.add_out_indices(out_indices);
  builder_.add_projections(projections);
  builder_.add_phi(phi);
  builder_.add_theta(theta);
  return builder_.Finish();
}

inline flatbuffers::Offset<Rectangle> CreateRectangleDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const FlatBuffers::Id *theta = nullptr,
    const FlatBuffers::Id *phi = nullptr,
    const std::vector<flatbuffers::Offset<FlatBuffers::Projection>> *projections = nullptr,
    const std::vector<uint32_t> *out_indices = nullptr) {
  auto projections__ = projections ? _fbb.CreateVector<flatbuffers::Offset<FlatBuffers::Projection>>(*projections) : 0;
  auto out_indices__ = out_indices ? _fbb.CreateVector<uint32_t>(*out_indices) : 0;
  return FlatBuffers::CreateRectangle(
      _fbb,
      theta,
      phi,
      projections__,
      out_indices__);
}

struct Box FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef BoxBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_THETA = 4,
    VT_PHI = 6,
    VT_ALPHA = 8,
    VT_PROJECTIONS = 10,
    VT_PROJECTION = 12,
    VT_RECTANGLES = 14,
    VT_TERMINAL = 16,
    VT_IN_INDEX = 18
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
  const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Rectangle>> *rectangles() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Rectangle>> *>(VT_RECTANGLES);
  }
  bool terminal() const {
    return GetField<uint8_t>(VT_TERMINAL, 0) != 0;
  }
  int32_t in_index() const {
    return GetField<int32_t>(VT_IN_INDEX, 0);
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
           VerifyOffset(verifier, VT_RECTANGLES) &&
           verifier.VerifyVector(rectangles()) &&
           verifier.VerifyVectorOfTables(rectangles()) &&
           VerifyField<uint8_t>(verifier, VT_TERMINAL, 1) &&
           VerifyField<int32_t>(verifier, VT_IN_INDEX, 4) &&
           verifier.EndTable();
  }
};

struct BoxBuilder {
  typedef Box Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_theta(const FlatBuffers::Id *theta) {
    fbb_.AddStruct(Box::VT_THETA, theta);
  }
  void add_phi(const FlatBuffers::Id *phi) {
    fbb_.AddStruct(Box::VT_PHI, phi);
  }
  void add_alpha(const FlatBuffers::Id *alpha) {
    fbb_.AddStruct(Box::VT_ALPHA, alpha);
  }
  void add_projections(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>>> projections) {
    fbb_.AddOffset(Box::VT_PROJECTIONS, projections);
  }
  void add_projection(flatbuffers::Offset<FlatBuffers::Polygon> projection) {
    fbb_.AddOffset(Box::VT_PROJECTION, projection);
  }
  void add_rectangles(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Rectangle>>> rectangles) {
    fbb_.AddOffset(Box::VT_RECTANGLES, rectangles);
  }
  void add_terminal(bool terminal) {
    fbb_.AddElement<uint8_t>(Box::VT_TERMINAL, static_cast<uint8_t>(terminal), 0);
  }
  void add_in_index(int32_t in_index) {
    fbb_.AddElement<int32_t>(Box::VT_IN_INDEX, in_index, 0);
  }
  explicit BoxBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Box> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Box>(end);
    return o;
  }
};

inline flatbuffers::Offset<Box> CreateBox(
    flatbuffers::FlatBufferBuilder &_fbb,
    const FlatBuffers::Id *theta = nullptr,
    const FlatBuffers::Id *phi = nullptr,
    const FlatBuffers::Id *alpha = nullptr,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Projection>>> projections = 0,
    flatbuffers::Offset<FlatBuffers::Polygon> projection = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Rectangle>>> rectangles = 0,
    bool terminal = false,
    int32_t in_index = 0) {
  BoxBuilder builder_(_fbb);
  builder_.add_in_index(in_index);
  builder_.add_rectangles(rectangles);
  builder_.add_projection(projection);
  builder_.add_projections(projections);
  builder_.add_alpha(alpha);
  builder_.add_phi(phi);
  builder_.add_theta(theta);
  builder_.add_terminal(terminal);
  return builder_.Finish();
}

inline flatbuffers::Offset<Box> CreateBoxDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const FlatBuffers::Id *theta = nullptr,
    const FlatBuffers::Id *phi = nullptr,
    const FlatBuffers::Id *alpha = nullptr,
    const std::vector<flatbuffers::Offset<FlatBuffers::Projection>> *projections = nullptr,
    flatbuffers::Offset<FlatBuffers::Polygon> projection = 0,
    const std::vector<flatbuffers::Offset<FlatBuffers::Rectangle>> *rectangles = nullptr,
    bool terminal = false,
    int32_t in_index = 0) {
  auto projections__ = projections ? _fbb.CreateVector<flatbuffers::Offset<FlatBuffers::Projection>>(*projections) : 0;
  auto rectangles__ = rectangles ? _fbb.CreateVector<flatbuffers::Offset<FlatBuffers::Rectangle>>(*rectangles) : 0;
  return FlatBuffers::CreateBox(
      _fbb,
      theta,
      phi,
      alpha,
      projections__,
      projection,
      rectangles__,
      terminal,
      in_index);
}

struct Boxes FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef BoxesBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_DESCRIPTION = 4,
    VT_HOLE = 6,
    VT_PLUG = 8,
    VT_BOXES = 10
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
  const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box>> *boxes() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box>> *>(VT_BOXES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_DESCRIPTION) &&
           verifier.VerifyString(description()) &&
           VerifyOffset(verifier, VT_HOLE) &&
           verifier.VerifyTable(hole()) &&
           VerifyOffset(verifier, VT_PLUG) &&
           verifier.VerifyTable(plug()) &&
           VerifyOffset(verifier, VT_BOXES) &&
           verifier.VerifyVector(boxes()) &&
           verifier.VerifyVectorOfTables(boxes()) &&
           verifier.EndTable();
  }
};

struct BoxesBuilder {
  typedef Boxes Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_description(flatbuffers::Offset<flatbuffers::String> description) {
    fbb_.AddOffset(Boxes::VT_DESCRIPTION, description);
  }
  void add_hole(flatbuffers::Offset<FlatBuffers::Polyhedron> hole) {
    fbb_.AddOffset(Boxes::VT_HOLE, hole);
  }
  void add_plug(flatbuffers::Offset<FlatBuffers::Polyhedron> plug) {
    fbb_.AddOffset(Boxes::VT_PLUG, plug);
  }
  void add_boxes(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box>>> boxes) {
    fbb_.AddOffset(Boxes::VT_BOXES, boxes);
  }
  explicit BoxesBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Boxes> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Boxes>(end);
    return o;
  }
};

inline flatbuffers::Offset<Boxes> CreateBoxes(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> description = 0,
    flatbuffers::Offset<FlatBuffers::Polyhedron> hole = 0,
    flatbuffers::Offset<FlatBuffers::Polyhedron> plug = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FlatBuffers::Box>>> boxes = 0) {
  BoxesBuilder builder_(_fbb);
  builder_.add_boxes(boxes);
  builder_.add_plug(plug);
  builder_.add_hole(hole);
  builder_.add_description(description);
  return builder_.Finish();
}

inline flatbuffers::Offset<Boxes> CreateBoxesDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *description = nullptr,
    flatbuffers::Offset<FlatBuffers::Polyhedron> hole = 0,
    flatbuffers::Offset<FlatBuffers::Polyhedron> plug = 0,
    const std::vector<flatbuffers::Offset<FlatBuffers::Box>> *boxes = nullptr) {
  auto description__ = description ? _fbb.CreateString(description) : 0;
  auto boxes__ = boxes ? _fbb.CreateVector<flatbuffers::Offset<FlatBuffers::Box>>(*boxes) : 0;
  return FlatBuffers::CreateBoxes(
      _fbb,
      description__,
      hole,
      plug,
      boxes__);
}

inline const FlatBuffers::Boxes *GetBoxes(const void *buf) {
  return flatbuffers::GetRoot<FlatBuffers::Boxes>(buf);
}

inline const FlatBuffers::Boxes *GetSizePrefixedBoxes(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<FlatBuffers::Boxes>(buf);
}

inline bool VerifyBoxesBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FlatBuffers::Boxes>(nullptr);
}

inline bool VerifySizePrefixedBoxesBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FlatBuffers::Boxes>(nullptr);
}

inline void FinishBoxesBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FlatBuffers::Boxes> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedBoxesBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FlatBuffers::Boxes> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace FlatBuffers

#endif  // FLATBUFFERS_GENERATED_FLATBUFFERS_FLATBUFFERS_H_
