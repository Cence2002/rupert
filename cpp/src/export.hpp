#pragma once

#include "flatbuffers/flatbuffers_generated.h"
#include <fstream>

#include "geometry/geometry.hpp"
#include "cover.hpp"

struct ProjectionBuilder {
private:
    std::vector<FlatBuffers::Vector2> vertices_;

public:
    template<IntervalType Interval>
    void add_vertex(const IntervalVector2<Interval>& vertex) {
        vertices_.emplace_back(
            vertex.x().mid().float_value(),
            vertex.y().mid().float_value()
        );
    }

    auto build(flatbuffers::FlatBufferBuilder& builder) {
        const auto projection = FlatBuffers::CreateProjection(builder, builder.CreateVectorOfStructs(vertices_));
        vertices_.clear();
        return projection;
    }
};

struct Box2Builder {
private:
    FlatBuffers::Id theta_;
    FlatBuffers::Id phi_;
    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> projections_;
    std::vector<uint8_t> out_;

public:
    ProjectionBuilder projection_builder;

    void set_box2(const Box2& box2) {
        theta_ = FlatBuffers::Id(
            box2.theta_id().bits(),
            box2.theta_id().depth(),
            FlatBuffers::Interval(
                box2.theta<FloatInterval>().min().float_value(),
                box2.theta<FloatInterval>().max().float_value()
            )
        );
        phi_ = FlatBuffers::Id(
            box2.phi_id().bits(),
            box2.phi_id().depth(),
            FlatBuffers::Interval(
                box2.phi<FloatInterval>().min().float_value(),
                box2.phi<FloatInterval>().max().float_value()
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
        const auto box2 = FlatBuffers::CreateBox2(
            builder,
            &theta_,
            &phi_,
            builder.CreateVector(projections_),
            builder.CreateVector(out_)
        );
        projections_.clear();
        out_.clear();
        return box2;
    }
};

struct Box3Builder {
private:
    FlatBuffers::Id theta_;
    FlatBuffers::Id phi_;
    FlatBuffers::Id alpha_;
    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> projections_;
    flatbuffers::Offset<FlatBuffers::Polygon> projection_;
    std::vector<flatbuffers::Offset<FlatBuffers::Box2>> box2s_;
    bool complete_ = false;
    int32_t in_ = -1;

public:
    ProjectionBuilder projection_builder;
    Box2Builder box2_builder;

    void set_box3(const Box3& box3) {
        theta_ = FlatBuffers::Id(
            box3.theta_id().bits(),
            box3.theta_id().depth(),
            FlatBuffers::Interval(
                box3.theta<FloatInterval>().min().float_value(),
                box3.theta<FloatInterval>().max().float_value()
            )
        );
        phi_ = FlatBuffers::Id(
            box3.phi_id().bits(),
            box3.phi_id().depth(),
            FlatBuffers::Interval(
                box3.phi<FloatInterval>().min().float_value(),
                box3.phi<FloatInterval>().max().float_value()
            )
        );
        alpha_ = FlatBuffers::Id(
            box3.alpha_id().bits(),
            box3.alpha_id().depth(),
            FlatBuffers::Interval(
                box3.alpha<FloatInterval>().min().float_value(),
                box3.alpha<FloatInterval>().max().float_value()
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
            edges.emplace_back(
                FlatBuffers::Vector2(edge.point().x().mid().float_value(), edge.point().y().mid().float_value()),
                FlatBuffers::Vector2(edge.direction().x().mid().float_value(), edge.direction().y().mid().float_value())
            );
        }
        projection_ = FlatBuffers::CreatePolygon(builder, builder.CreateVectorOfStructs(edges));
    }

    void add_box2(flatbuffers::FlatBufferBuilder& builder) {
        box2s_.push_back(box2_builder.build(builder));
    }

    void set_complete(const bool complete) {
        complete_ = complete;
    }

    void save_in() {
        in_ = static_cast<int32_t>(box2s_.size()) - 1;
    }

    auto build(flatbuffers::FlatBufferBuilder& builder) {
        const auto box3 = FlatBuffers::CreateBox3(
            builder,
            &theta_,
            &phi_,
            &alpha_,
            builder.CreateVector(projections_),
            projection_,
            builder.CreateVector(box2s_),
            complete_,
            in_
        );
        projections_.clear();
        box2s_.clear();
        complete_ = false;
        in_ = -1;
        return box3;
    }
};

struct CoverBuilder {
private:
    std::string description;
    flatbuffers::Offset<FlatBuffers::Polyhedron> hole;
    flatbuffers::Offset<FlatBuffers::Polyhedron> plug;
    std::vector<flatbuffers::Offset<FlatBuffers::Box3>> box3s;

public:
    Box3Builder box3_builder;

    void set_description(const std::string& description) {
        this->description = description;
    }

    template<IntervalType Interval>
    void set_hole(flatbuffers::FlatBufferBuilder& builder, const Polyhedron<Interval>& polyhedron) {
        std::vector<FlatBuffers::Vector3> vertices;
        for(const auto& vertex: polyhedron.vertices()) {
            vertices.emplace_back(
                vertex.x().mid().float_value(),
                vertex.y().mid().float_value(),
                vertex.z().mid().float_value()
            );
        }
        hole = FlatBuffers::CreatePolyhedron(builder, builder.CreateVectorOfStructs(vertices));
    }

    template<IntervalType Interval>
    void set_plug(flatbuffers::FlatBufferBuilder& builder, const Polyhedron<Interval>& polyhedron) {
        std::vector<FlatBuffers::Vector3> vertices;
        for(const auto& vertex: polyhedron.vertices()) {
            vertices.emplace_back(
                vertex.x().mid().float_value(),
                vertex.y().mid().float_value(),
                vertex.z().mid().float_value()
            );
        }
        plug = FlatBuffers::CreatePolyhedron(builder, builder.CreateVectorOfStructs(vertices));
    }

    void add_box3(flatbuffers::FlatBufferBuilder& builder) {
        box3s.push_back(box3_builder.build(builder));
    }

    void build(flatbuffers::FlatBufferBuilder& builder) {
        const auto descriptionOffset = builder.CreateString(description);
        const auto cover = FlatBuffers::CreateCover(
            builder,
            descriptionOffset,
            hole,
            plug,
            builder.CreateVector(box3s)
        );
        builder.Finish(cover);
    }
};

struct Exporter {
public:
    flatbuffers::FlatBufferBuilder builder;
    CoverBuilder cover_builder;

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
