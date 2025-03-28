#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "flatbuffers/flatbuffers_generated.h"
#pragma GCC diagnostic pop

#include "pipeline/config.hpp"
#include <fstream>

template<IntervalType Interval>
FlatBuffers::Vector to_flatbuffers_vector(const Vector<Interval>& vector) {
    return FlatBuffers::Vector(
        vector.x().mid().float_value(),
        vector.y().mid().float_value()
    );
}

template<IntervalType Interval>
FlatBuffers::Vertex to_flatbuffers_vertex(const Vertex<Interval>& vertex) {
    return FlatBuffers::Vertex(
        vertex.x().mid().float_value(),
        vertex.y().mid().float_value(),
        vertex.z().mid().float_value()
    );
}

template<IntervalType Interval>
FlatBuffers::Edge to_flatbuffers_edge(const Edge<Interval>& edge) {
    return FlatBuffers::Edge(
        to_flatbuffers_vector(edge.from()),
        to_flatbuffers_vector(edge.to())
    );
}

template<IntervalType Interval>
FlatBuffers::Interval to_flatbuffers_interval(const Interval& interval) {
    return FlatBuffers::Interval(
        interval.min().float_value(),
        interval.max().float_value()
    );
}

template<IntervalType Interval>
FlatBuffers::Id to_flatbuffers_id(const Id& id, const Interval& interval) {
    return FlatBuffers::Id(
        id.depth(),
        id.bits(),
        to_flatbuffers_interval(interval)
    );
}

struct ProjectionBuilder {
private:
    flatbuffers::FlatBufferBuilder& builder_;

    std::vector<FlatBuffers::Vector> vectors_;

public:
    explicit ProjectionBuilder(flatbuffers::FlatBufferBuilder& builder) : builder_(builder),
                                                                          vectors_() {}

    template<IntervalType Interval>
    void add_vertex(const Vector<Interval>& vector) {
        vectors_.push_back(to_flatbuffers_vector(vector));
    }

    flatbuffers::Offset<FlatBuffers::Projection> build() {
        const auto projection = FlatBuffers::CreateProjection(builder_, builder_.CreateVectorOfStructs(vectors_));
        vectors_.clear();
        return projection;
    }

    size_t size() const {
        return vectors_.size();
    }
};

struct RectangleBuilder {
private:
    flatbuffers::FlatBufferBuilder& builder_;

    FlatBuffers::Id theta_;
    FlatBuffers::Id phi_;
    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> projections_;
    std::vector<uint32_t> out_indices_;

public:
    ProjectionBuilder projection_builder;

    explicit RectangleBuilder(flatbuffers::FlatBufferBuilder& builder) : builder_(builder),
                                                                         theta_(),
                                                                         phi_(),
                                                                         projections_(),
                                                                         out_indices_(),
                                                                         projection_builder(builder) {}

    void set_rectangle(const Rectangle& rectangle) {
        theta_ = to_flatbuffers_id(rectangle.theta_id(), rectangle.theta<FloatInterval>());
        phi_ = to_flatbuffers_id(rectangle.phi_id(), rectangle.phi<FloatInterval>());
    }

    void add_projection() {
        projections_.push_back(projection_builder.build());
    }

    void add_out_index(const uint32_t out) {
        out_indices_.push_back(out);
    }

    auto build() {
        const flatbuffers::Offset<FlatBuffers::Rectangle> rectangle = FlatBuffers::CreateRectangle(
            builder_,
            &theta_,
            &phi_,
            builder_.CreateVector(projections_),
            builder_.CreateVector(out_indices_)
        );
        projections_.clear();
        out_indices_.clear();
        return rectangle;
    }

    void add_last_as_out_index() {
        out_indices_.push_back(static_cast<uint32_t>(projections_.size() - 1));
    }
};

struct BoxBuilder {
private:
    flatbuffers::FlatBufferBuilder& builder_;

    FlatBuffers::Id theta_;
    FlatBuffers::Id phi_;
    FlatBuffers::Id alpha_;
    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> projections_;
    flatbuffers::Offset<FlatBuffers::Polygon> projection_;
    std::vector<flatbuffers::Offset<FlatBuffers::Rectangle>> rectangles_;
    bool terminal_;
    int32_t in_index_;

public:
    ProjectionBuilder projection_builder;
    RectangleBuilder rectangle_builder;

    explicit BoxBuilder(flatbuffers::FlatBufferBuilder& builder) : builder_(builder),
                                                                   theta_(),
                                                                   phi_(),
                                                                   alpha_(),
                                                                   projections_(),
                                                                   projection_(),
                                                                   rectangles_(),
                                                                   terminal_(false),
                                                                   in_index_(-1),
                                                                   projection_builder(builder),
                                                                   rectangle_builder(builder) {}

    void set_box(const Box& box) {
        theta_ = to_flatbuffers_id(box.theta_id(), box.theta<FloatInterval>());
        phi_ = to_flatbuffers_id(box.phi_id(), box.phi<FloatInterval>());
        alpha_ = to_flatbuffers_id(box.alpha_id(), box.alpha<FloatInterval>());
    }

    void add_projection() {
        projections_.push_back(projection_builder.build());
    }

    template<IntervalType Interval>
    void set_projection(const Polygon<Interval>& polygon) {
        std::vector<FlatBuffers::Edge> edges;
        for(const auto& edge: polygon.edges()) {
            const auto flatbuffers_edge = FlatBuffers::Edge(
                FlatBuffers::Vector(edge.from().x().mid().float_value(), edge.from().y().mid().float_value()),
                FlatBuffers::Vector(edge.to().x().mid().float_value(), edge.to().y().mid().float_value())
            );
            edges.push_back(flatbuffers_edge);
        }
        projection_ = FlatBuffers::CreatePolygon(builder_, builder_.CreateVectorOfStructs(edges));
    }

    void add_rectangle() {
        rectangles_.push_back(rectangle_builder.build());
    }

    void set_terminal(const bool terminal) {
        terminal_ = terminal;
    }

    void set_last_as_in_index() {
        in_index_ = static_cast<int32_t>(rectangles_.size()) - 1;
    }

    auto build() {
        const flatbuffers::Offset<FlatBuffers::Box> box = FlatBuffers::CreateBox(
            builder_,
            &theta_,
            &phi_,
            &alpha_,
            builder_.CreateVector(projections_),
            projection_,
            builder_.CreateVector(rectangles_),
            terminal_,
            in_index_
        );
        projections_.clear();
        rectangles_.clear();
        terminal_ = false;
        in_index_ = -1;
        return box;
    }
};

struct DebugBuilder {
private:
    flatbuffers::FlatBufferBuilder& builder_;

    flatbuffers::Offset<FlatBuffers::Polyhedron> hole_;
    flatbuffers::Offset<FlatBuffers::Polyhedron> plug_;
    std::vector<flatbuffers::Offset<FlatBuffers::Box>> boxes_;

public:
    BoxBuilder box_builder;

    explicit DebugBuilder(flatbuffers::FlatBufferBuilder& builder) : builder_(builder), hole_(), plug_(), boxes_(), box_builder(builder_) {}

    template<IntervalType Interval>
    void set_hole(const Polyhedron<Interval>& polyhedron) {
        std::vector<FlatBuffers::Vertex> vertices;
        std::ranges::transform(polyhedron.vertices(), std::back_inserter(vertices), to_flatbuffers_vertex<Interval>);
        hole_ = FlatBuffers::CreatePolyhedron(builder_, builder_.CreateVectorOfStructs(vertices));
    }

    template<IntervalType Interval>
    void set_plug(const Polyhedron<Interval>& polyhedron) {
        std::vector<FlatBuffers::Vertex> vertices;
        std::ranges::transform(polyhedron.vertices(), std::back_inserter(vertices), to_flatbuffers_vertex<Interval>);
        plug_ = FlatBuffers::CreatePolyhedron(builder_, builder_.CreateVectorOfStructs(vertices));
    }

    void add_box() {
        boxes_.push_back(box_builder.build());
    }

    void build() const {
        const flatbuffers::Offset<FlatBuffers::Debug> boxes = FlatBuffers::CreateDebug(
            builder_,
            hole_,
            plug_,
            builder_.CreateVector(boxes_)
        );
        builder_.Finish(boxes);
    }
};

template<IntervalType Interval>
struct DebugExporter {
private:
    const Config<Interval>& config_;
    flatbuffers::FlatBufferBuilder builder_;

public:
    DebugBuilder debug_builder;

    explicit DebugExporter(const Config<Interval>& config) : config_(config), builder_(), debug_builder(builder_) {}

    void export_debug() {
        debug_builder.build();

        const std::filesystem::path path = config_.debug_path();
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        file.write(reinterpret_cast<const char*>(builder_.GetBufferPointer()), builder_.GetSize());

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported debug data to " << path << std::endl;
    }
};
