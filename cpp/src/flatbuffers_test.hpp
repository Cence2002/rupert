#pragma once

#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/flatbuffers_generated.h"
#include <fstream>

void generate_test() {
    // Create a FlatBufferBuilder with an initial size (in bytes).
    flatbuffers::FlatBufferBuilder builder(1024);

    // --- Create the Polyhedron for "hole" and "plug" ---
    // (Polyhedron contains a vector of Vector3 structs.)
    std::vector<FlatBuffers::Vector3> polyhedronVertices = {
            {0.0, 0.0, 0.0},
            {1.0, 0.0, 0.0},
            {1.0, 1.0, 0.0},
            {0.0, 1.0, 1.0}
        };
    // Create the vector of structs (for Vector3) in the buffer.
    auto verticesOffset = builder.CreateVectorOfStructs(polyhedronVertices);
    // Both "hole" and "plug" use the same vertex data here.
    auto hole = FlatBuffers::CreatePolyhedron(builder, verticesOffset);
    auto plug = FlatBuffers::CreatePolyhedron(builder, verticesOffset);

    // --- Create a Polygon (used later as the "projection" field in Box3) ---
    // A Polygon is defined as a table containing a vector of Line structs.
    std::vector<FlatBuffers::Line> polygonLines = {
            {{0.0, 0.0}, {1.0, 0.0}},
            {{0.0, 1.0}, {1.0, 1.0}}
        };
    auto linesOffset = builder.CreateVectorOfStructs(polygonLines);
    auto polygon = FlatBuffers::CreatePolygon(builder, linesOffset);

    // --- Create two Projection tables (each with a vector of Vector2 structs) ---
    // First Projection:
    std::vector<FlatBuffers::Vector2> proj1Vertices = {
            {0.0, 0.0},
            {1.0, 1.0}
        };
    auto proj1VerticesOffset = builder.CreateVectorOfStructs(proj1Vertices);
    auto projection1 = FlatBuffers::CreateProjection(builder, proj1VerticesOffset);

    // Second Projection:
    std::vector<FlatBuffers::Vector2> proj2Vertices = {
            {2.0, 2.0},
            {3.0, 3.0}
        };
    auto proj2VerticesOffset = builder.CreateVectorOfStructs(proj2Vertices);
    auto projection2 = FlatBuffers::CreateProjection(builder, proj2VerticesOffset);

    // --- Create a Box2 instance ---
    // Box2 contains two Id structs, a vector of Projection tables, and a vector of uint8.
    FlatBuffers::Id box2_phi = {10, 0, {1.0, 2.0}};
    FlatBuffers::Id box2_theta = {20, 0, {2.0, 3.0}};

    // For Box2, let’s use the first Projection.
    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> box2Projections = {projection1};
    auto box2ProjectionsOffset = builder.CreateVector(box2Projections);

    // A small vector of uint8 values for the "out" field.
    std::vector<uint8_t> box2Out = {1, 2, 3};
    auto box2OutOffset = builder.CreateVector(box2Out);

    auto box2 = FlatBuffers::CreateBox2(builder, &box2_phi, &box2_theta, box2ProjectionsOffset, box2OutOffset);

    // --- Create a Box3 instance ---
    // Box3 contains three Id structs, a vector of Projection tables, a Polygon, a vector of Box2 tables,
    // a boolean flag, and a vector of uint8.
    FlatBuffers::Id box3_phi = {100, 1, {0.0, 10.0}};
    FlatBuffers::Id box3_theta = {200, 2, {5.0, 15.0}};
    FlatBuffers::Id box3_alpha = {300, 3, {10.0, 20.0}};

    // For Box3, use the second Projection.
    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> box3Projections = {projection2};
    auto box3ProjectionsOffset = builder.CreateVector(box3Projections);

    // Include the earlier-created Polygon (with lines) as the "projection" field.
    // Also, let Box3 reference one Box2 in its "box2s" field.
    std::vector<flatbuffers::Offset<FlatBuffers::Box2>> box2Vector = {box2};
    auto box2VectorOffset = builder.CreateVector(box2Vector);

    // A vector of uint8 for the "in" field.
    std::vector<uint8_t> box3In = {4, 5};
    auto box3InOffset = builder.CreateVector(box3In);

    // The last field "complete" is a boolean.
    auto box3 = FlatBuffers::CreateBox3(builder,
                                        &box3_phi,
                                        &box3_theta,
                                        &box3_alpha,
                                        box3ProjectionsOffset,
                                        polygon,
                                        box2VectorOffset,
                                        true, // complete
                                        box3InOffset);

    // --- Create the Cover table (the root object) ---
    // Cover has a description, a hole (Polyhedron), a plug (Polyhedron), and a vector of Box3 tables.
    std::vector<flatbuffers::Offset<FlatBuffers::Box3>> box3s = {box3};
    auto box3sOffset = builder.CreateVector(box3s);

    auto descriptionOffset = builder.CreateString("Test Cover Example");
    auto cover = FlatBuffers::CreateCover(builder, descriptionOffset, hole, plug, box3sOffset);

    // Mark the Cover as the root object.
    builder.Finish(cover);

    // --- Write the binary FlatBuffer to file ---
    std::ofstream outFile("test.bin", std::ios::binary);
    if(!outFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
    outFile.close();

    std::cout << "FlatBuffer binary saved to test.bin" << std::endl;
}
