#pragma once

#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/flatbuffers_generated.h"
#include <fstream>

void generate_test() {
    flatbuffers::FlatBufferBuilder builder(1024);

    std::vector<FlatBuffers::Vector3> holeVertices = {
        {1.0, 1.0, 1.0},
        {-1.0, 1.0, 1.0},
        {-1.0, -1.0, 1.0},
        {1.0, -1.0, 1.0},
        {1.0, 1.0, -1.0},
        {-1.0, 1.0, -1.0},
        {-1.0, -1.0, -1.0},
        {1.0, -1.0, -1.0},
    };
    auto hole = FlatBuffers::CreatePolyhedron(builder, builder.CreateVectorOfStructs(holeVertices));

    std::vector<FlatBuffers::Vector3> plugVertices = {
        {1.0, 1.0, 1.0},
        {-1.0, -1.0, 1.0},
        {1.0, -1.0, -1.0},
        {-1.0, 1.0, -1.0},
    };
    auto plug = FlatBuffers::CreatePolyhedron(builder, builder.CreateVectorOfStructs(plugVertices));

    std::vector<FlatBuffers::Line> polygonLines = {
            {{0.0, 0.0}, {1.0, 0.0}},
            {{1.0, 0.0}, {1.0, 1.0}},
            {{1.0, 1.0}, {0.0, 1.0}},
            {{0.0, 1.0}, {0.0, 0.0}},
            {{0.0, 1.0}, {0.0, 0.5}},
            {{0.0, 0.5}, {0.0, 0.0}},
        };
    auto polygon = FlatBuffers::CreatePolygon(builder, builder.CreateVectorOfStructs(polygonLines));

    std::vector<FlatBuffers::Vector2> proj1Vertices = {
            {0.0, 0.0},
            {1.0, 1.0},
            {1.0, 0.0},
        };
    auto projection1 = FlatBuffers::CreateProjection(builder, builder.CreateVectorOfStructs(proj1Vertices));

    std::vector<FlatBuffers::Vector2> proj2Vertices = {
        {2.0, 2.0},
        {3.0, 3.0},
        {-1.0, 2.0}
    };
    auto projection2 = FlatBuffers::CreateProjection(builder, builder.CreateVectorOfStructs(proj2Vertices));

    FlatBuffers::Id box2_phi = {0b101, 3, {1.0, 3.0}};
    FlatBuffers::Id box2_theta = {0b001, 3, {2.0, 3.0}};

    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> box2Projections = {projection1};
    auto box2ProjectionsOffset = builder.CreateVector(box2Projections);

    std::vector<uint8_t> box2Out = {1, 2, 4};
    auto box2OutOffset = builder.CreateVector(box2Out);

    auto box2 = FlatBuffers::CreateBox2(builder, &box2_phi, &box2_theta, box2ProjectionsOffset, box2OutOffset);

    FlatBuffers::Id box3_phi = {0b101, 3, {2.0, 4.0}};
    FlatBuffers::Id box3_theta = {0b111, 3, {2.0, 3.0}};
    FlatBuffers::Id box3_alpha = {0b100, 3, {0.1, 1.0}};

    std::vector<flatbuffers::Offset<FlatBuffers::Projection>> box3Projections = {projection2};
    auto box3ProjectionsOffset = builder.CreateVector(box3Projections);

    std::vector<flatbuffers::Offset<FlatBuffers::Box2>> box2Vector = {box2};
    auto box2VectorOffset = builder.CreateVector(box2Vector);

    std::vector<uint8_t> box3In = {0};
    auto box3InOffset = builder.CreateVector(box3In);

    auto box3 = FlatBuffers::CreateBox3(builder,
                                        &box3_phi,
                                        &box3_theta,
                                        &box3_alpha,
                                        box3ProjectionsOffset,
                                        polygon,
                                        box2VectorOffset,
                                        true, // complete
                                        box3InOffset);

    std::vector<flatbuffers::Offset<FlatBuffers::Box3>> box3s = {box3};
    auto box3sOffset = builder.CreateVector(box3s);

    auto descriptionOffset = builder.CreateString("Test Cover Example");
    auto cover = FlatBuffers::CreateCover(builder, descriptionOffset, hole, plug, box3sOffset);

    builder.Finish(cover);

    std::ofstream outFile("../../web/static/test.bin", std::ios::binary);
    if(!outFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
    outFile.close();

    std::cout << "FlatBuffer binary saved to test.bin" << std::endl;
}
