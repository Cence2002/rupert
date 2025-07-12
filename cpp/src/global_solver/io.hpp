#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "flatbuffers/flatbuffers_generated.h"
#pragma GCC diagnostic pop

#include "global_solver/helpers.hpp"
#include "range/ranges.hpp"
#include <fstream>
#include <filesystem>

class EliminatedHoleOrientation {
    Range3 hole_orientation;
    std::vector<Range2> plug_orientations;
};

namespace Importer {
    inline uint32_t size_from_stream(std::istream& is) {
        uint32_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        return size;
    }

    inline Range id_from_stream(std::istream& is) {
        uint16_t packed;
        is.read(reinterpret_cast<char*>(&packed), sizeof(packed));
        return Range::unpack(packed);
    }

    inline Range3 box_from_stream(std::istream& is) {
        const Range theta_id = id_from_stream(is);
        const Range phi_id = id_from_stream(is);
        const Range alpha_id = id_from_stream(is);
        return Range3(theta_id, phi_id, alpha_id);
    }

    inline void restart(const std::filesystem::path& working_directory) {
        if(std::filesystem::exists(working_directory)) {
            std::filesystem::remove_all(working_directory);
        }
    }

    inline std::vector<Range3> import_boxes(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);
        if(!file.is_open()) {
            return {};
        }

        const uint32_t size = size_from_stream(file);
        std::vector<Range3> boxes;
        boxes.reserve(size);
        std::ranges::generate_n(std::back_inserter(boxes), size, [&] {
            return box_from_stream(file);
        });

        if(file.fail()) {
            throw std::runtime_error("Failed to read from " + path.string());
        }
        std::cout << "Imported " << size << " boxes from " << path << std::endl;

        return boxes;
    }
}

namespace Exporter {
    inline void size_to_stream(std::ostream& os, const uint32_t size) {
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }

    inline void float_to_stream(std::ostream& os, const double value) {
        os.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    template<IntervalType Interval>
    void vertex_to_stream(std::ostream& os, const Vector3<Interval>& vertex) {
        float_to_stream(os, vertex.x().mid().to_float());
        float_to_stream(os, vertex.y().mid().to_float());
        float_to_stream(os, vertex.z().mid().to_float());
    }

    template<IntervalType Interval>
    void polyhedron_to_stream(std::ostream& os, const Polyhedron<Interval>& polyhedron) {
        size_to_stream(os, static_cast<uint32_t>(polyhedron.vertices().size()));
        for(const Vector3<Interval>& vertex: polyhedron.vertices()) {
            vertex_to_stream(os, vertex);
        }
    }

    inline void id_to_stream(std::ostream& os, const Range& id) {
        const uint16_t packed = id.pack();
        os.write(reinterpret_cast<const char*>(&packed), sizeof(packed));
    }

    inline void box_to_stream(std::ostream& os, const Range3& range3) {
        id_to_stream(os, range3.theta_range());
        id_to_stream(os, range3.phi_range());
        id_to_stream(os, range3.alpha_range());
    }

    inline void rectangle_to_stream(std::ostream& os, const Range2& range2) {
        id_to_stream(os, range2.theta_range());
        id_to_stream(os, range2.phi_range());
    }

    inline void terminal_box_to_stream(std::ostream& os, const EliminatedHoleOrientation& terminal_box) {
        box_to_stream(os, terminal_box.hole_orientation);
        size_to_stream(os, static_cast<uint32_t>(terminal_box.plug_orientations.size()));
        for(const Range2& rectangle: terminal_box.plug_orientations) {
            rectangle_to_stream(os, rectangle);
        }
    }

    inline void create_working_directory(const std::filesystem::path& working_directory) {
        if(!std::filesystem::exists(working_directory)) {
            std::filesystem::create_directory(working_directory);
        }
    }

    template<IntervalType Interval>
    void export_polyhedra(const std::filesystem::path& path, const Polyhedron<Interval>& polyhedron) {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        polyhedron_to_stream(file, polyhedron);
        polyhedron_to_stream(file, polyhedron); // TODO remove this

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported polyhedra to " << path << std::endl;
    }

    inline void export_terminal_boxes(const std::filesystem::path& path, const std::vector<EliminatedHoleOrientation>& terminal_boxes) {
        std::ofstream file(path, std::ios::binary | std::ios::app);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        for(const EliminatedHoleOrientation& terminal_box: terminal_boxes) {
            terminal_box_to_stream(file, terminal_box);
        }

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported " << terminal_boxes.size() << " terminal boxes to " << path << std::endl;
    }

    inline void export_boxes(const std::filesystem::path& path, const std::vector<Range3>& boxes) {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        size_to_stream(file, static_cast<uint32_t>(boxes.size()));
        for(const Range3& box: boxes) {
            box_to_stream(file, box);
        }

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported " << boxes.size() << " boxes to " << path << std::endl;
    }
}

namespace DebugHelpers {
    template<IntervalType Interval>
    FlatBuffers::Vector to_flatbuffers_vector(const Vector2<Interval>& vector) {
        return FlatBuffers::Vector(
            vector.x().mid().to_float(),
            vector.y().mid().to_float()
        );
    }

    template<IntervalType Interval>
    FlatBuffers::Vertex to_flatbuffers_vertex(const Vector3<Interval>& vertex) {
        return FlatBuffers::Vertex(
            vertex.x().mid().to_float(),
            vertex.y().mid().to_float(),
            vertex.z().mid().to_float()
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
            interval.min().to_float(),
            interval.max().to_float()
        );
    }

    template<IntervalType Interval>
    FlatBuffers::Id to_flatbuffers_id(const Range& id, const Interval& interval) {
        return FlatBuffers::Id(
            id.depth(),
            id.bits(),
            to_flatbuffers_interval(interval)
        );
    }

    class ProjectionBuilder {
    private:
        flatbuffers::FlatBufferBuilder& builder_;

        std::vector<FlatBuffers::Vector> vectors_;

    public:
        explicit ProjectionBuilder(flatbuffers::FlatBufferBuilder& builder) : builder_(builder),
                                                                              vectors_() {}

        template<IntervalType Interval>
        void add_vertex(const Vector2<Interval>& vector) {
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

    class RectangleBuilder {
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

        void set_rectangle(const Range2& rectangle) {
            theta_ = to_flatbuffers_id(rectangle.theta_range(), rectangle.theta<FloatInterval>());
            phi_ = to_flatbuffers_id(rectangle.phi_range(), rectangle.phi<FloatInterval>());
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

    class BoxBuilder {
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

        void set_box(const Range3& box) {
            theta_ = to_flatbuffers_id(box.theta_range(), box.theta<FloatInterval>());
            phi_ = to_flatbuffers_id(box.phi_range(), box.phi<FloatInterval>());
            alpha_ = to_flatbuffers_id(box.alpha_range(), box.alpha<FloatInterval>());
        }

        void add_projection() {
            projections_.push_back(projection_builder.build());
        }

        template<IntervalType Interval>
        void set_projection(const Polygon<Interval>& polygon) {
            std::vector<FlatBuffers::Edge> edges;
            for(const auto& edge: polygon.edges()) {
                const auto flatbuffers_edge = FlatBuffers::Edge(
                    FlatBuffers::Vector(edge.from().x().mid().to_float(), edge.from().y().mid().to_float()),
                    FlatBuffers::Vector(edge.to().x().mid().to_float(), edge.to().y().mid().to_float())
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

    class DebugBuilder {
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
}

template<IntervalType Interval>
class DebugExporter {
    flatbuffers::FlatBufferBuilder builder_;

public:
    DebugHelpers::DebugBuilder debug_builder;

    explicit DebugExporter() : builder_(), debug_builder(builder_) {}

    void export_debug(const std::filesystem::path& path) const {
        debug_builder.build();

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
