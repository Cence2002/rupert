#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "flatbuffers/flatbuffers_generated.h"
#pragma GCC diagnostic pop

#include "geometry/geometry.hpp"
#include "pipeline/pipeline.hpp"
#include <fstream>

struct ProjectionBuilder {
private:
    std::vector<FlatBuffers::Vector> vectors_;

public:
    explicit ProjectionBuilder() : vectors_() {}

    template<IntervalType Interval>
    void add_vertex(const Vector<Interval>& vector) {
        vectors_.emplace_back(
            vector.x().mid().float_value(),
            vector.y().mid().float_value()
        );
    }

    auto build(flatbuffers::FlatBufferBuilder& builder) {
        const auto projection = FlatBuffers::CreateProjection(builder, builder.CreateVectorOfStructs(vectors_));
        vectors_.clear();
        return projection;
    }

    size_t size() const {
        return vectors_.size();
    }
};

struct RectangleBuilder {
private:
    FlatBuffers::Id theta_;
    FlatBuffers::Id phi_;
    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> projections_;
    std::vector<uint8_t> out_;

public:
    ProjectionBuilder projection_builder;

    explicit RectangleBuilder() : theta_(), phi_(), projections_(), out_(), projection_builder() {}

    void set_rectangle(const Rectangle& rectangle) {
        theta_ = FlatBuffers::Id(
            rectangle.theta_id().bits(),
            rectangle.theta_id().depth(),
            FlatBuffers::Interval(
                rectangle.theta<FloatInterval>().min().float_value(),
                rectangle.theta<FloatInterval>().max().float_value()
            )
        );
        phi_ = FlatBuffers::Id(
            rectangle.phi_id().bits(),
            rectangle.phi_id().depth(),
            FlatBuffers::Interval(
                rectangle.phi<FloatInterval>().min().float_value(),
                rectangle.phi<FloatInterval>().max().float_value()
            )
        );
    }

    void add_projection(flatbuffers::FlatBufferBuilder& builder) {
        projections_.push_back(projection_builder.build(builder));
    }

    void add_out(const uint8_t out) {
        out_.push_back(out);
    }

    auto build(flatbuffers::FlatBufferBuilder& builder) {
        const auto rectangle = FlatBuffers::CreateRectangle(
            builder,
            &theta_,
            &phi_,
            builder.CreateVector(projections_),
            builder.CreateVector(out_)
        );
        projections_.clear();
        out_.clear();
        return rectangle;
    }

    void add_last_as_out() {
        out_.push_back(static_cast<uint8_t>(projections_.size() - 1));
    }
};

struct BoxBuilder {
private:
    FlatBuffers::Id theta_;
    FlatBuffers::Id phi_;
    FlatBuffers::Id alpha_;
    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> projections_;
    flatbuffers::Offset<FlatBuffers::Polygon> projection_;
    std::vector<flatbuffers::Offset<FlatBuffers::Rectangle>> rectangles_;
    bool complete_ = false;
    int32_t in_ = -1;

public:
    ProjectionBuilder projection_builder;
    RectangleBuilder rectangle_builder;

    BoxBuilder() : theta_(), phi_(), alpha_(), projections_(), projection_(), rectangles_(), projection_builder(), rectangle_builder() {}

    void set_box(const Box& box) {
        theta_ = FlatBuffers::Id(
            box.theta_id().bits(),
            box.theta_id().depth(),
            FlatBuffers::Interval(
                box.theta<FloatInterval>().min().float_value(),
                box.theta<FloatInterval>().max().float_value()
            )
        );
        phi_ = FlatBuffers::Id(
            box.phi_id().bits(),
            box.phi_id().depth(),
            FlatBuffers::Interval(
                box.phi<FloatInterval>().min().float_value(),
                box.phi<FloatInterval>().max().float_value()
            )
        );
        alpha_ = FlatBuffers::Id(
            box.alpha_id().bits(),
            box.alpha_id().depth(),
            FlatBuffers::Interval(
                box.alpha<FloatInterval>().min().float_value(),
                box.alpha<FloatInterval>().max().float_value()
            )
        );
    }

    void add_projection(flatbuffers::FlatBufferBuilder& builder) {
        projections_.push_back(projection_builder.build(builder));
    }

    template<IntervalType Interval>
    void set_projection(flatbuffers::FlatBufferBuilder& builder, const Polygon<Interval>& polygon) {
        std::vector<FlatBuffers::Edge> edges;
        for(const auto& edge: polygon.edges()) {
            const auto flatbuffers_edge = FlatBuffers::Edge(
                FlatBuffers::Vector(edge.from().x().mid().float_value(), edge.from().y().mid().float_value()),
                FlatBuffers::Vector(edge.to().x().mid().float_value(), edge.to().y().mid().float_value())
            );
            edges.push_back(flatbuffers_edge);
        }
        projection_ = FlatBuffers::CreatePolygon(builder, builder.CreateVectorOfStructs(edges));
    }

    void add_rectangle(flatbuffers::FlatBufferBuilder& builder) {
        rectangles_.push_back(rectangle_builder.build(builder));
    }

    void set_complete(const bool complete) {
        complete_ = complete;
    }

    void save_in() {
        in_ = static_cast<int32_t>(rectangles_.size()) - 1;
    }

    auto build(flatbuffers::FlatBufferBuilder& builder) {
        const auto box = FlatBuffers::CreateBox(
            builder,
            &theta_,
            &phi_,
            &alpha_,
            builder.CreateVector(projections_),
            projection_,
            builder.CreateVector(rectangles_),
            complete_,
            in_
        );
        projections_.clear();
        rectangles_.clear();
        complete_ = false;
        in_ = -1;
        return box;
    }
};

struct CoverBuilder {
private:
    std::string description_;
    flatbuffers::Offset<FlatBuffers::Polyhedron> hole_;
    flatbuffers::Offset<FlatBuffers::Polyhedron> plug_;
    std::vector<flatbuffers::Offset<FlatBuffers::Box>> boxes_;

public:
    BoxBuilder box_builder;

    explicit CoverBuilder() : description_(), hole_(), plug_(), boxes_(), box_builder() {}

    void set_description(const std::string& description) {
        description_ = description;
    }

    template<IntervalType Interval>
    void set_hole(flatbuffers::FlatBufferBuilder& builder, const Polyhedron<Interval>& polyhedron) {
        std::vector<FlatBuffers::Vertex> vertices;
        for(const auto& vertex: polyhedron.vertices()) {
            vertices.emplace_back(
                vertex.x().mid().float_value(),
                vertex.y().mid().float_value(),
                vertex.z().mid().float_value()
            );
        }
        hole_ = FlatBuffers::CreatePolyhedron(builder, builder.CreateVectorOfStructs(vertices));
    }

    template<IntervalType Interval>
    void set_plug(flatbuffers::FlatBufferBuilder& builder, const Polyhedron<Interval>& polyhedron) {
        std::vector<FlatBuffers::Vertex> vertices;
        for(const auto& vertex: polyhedron.vertices()) {
            vertices.emplace_back(
                vertex.x().mid().float_value(),
                vertex.y().mid().float_value(),
                vertex.z().mid().float_value()
            );
        }
        plug_ = FlatBuffers::CreatePolyhedron(builder, builder.CreateVectorOfStructs(vertices));
    }

    void add_box(flatbuffers::FlatBufferBuilder& builder) {
        boxes_.push_back(box_builder.build(builder));
    }

    void build(flatbuffers::FlatBufferBuilder& builder) {
        const auto descriptionOffset = builder.CreateString(description_);
        const auto cover = FlatBuffers::CreateCover(
            builder,
            descriptionOffset,
            hole_,
            plug_,
            builder.CreateVector(boxes_)
        );
        builder.Finish(cover);
    }
};

struct DebugExporter {
    flatbuffers::FlatBufferBuilder builder;
    CoverBuilder cover_builder;

    explicit DebugExporter() : builder(), cover_builder() {}

    void save(const std::string& filename) {
        cover_builder.build(builder);

        std::ofstream outFile(filename, std::ios::binary);
        if(!outFile) {
            std::cerr << "Error opening file for writing." << std::endl;
            return;
        }
        outFile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
        outFile.close();

        std::cout << "FlatBuffer binary saved to " << filename << std::endl;
    }
};
