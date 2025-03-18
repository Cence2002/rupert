import * as flatbuffers from 'flatbuffers';

import {Debug} from '$lib/flatbuffers/flatbuffers_generated';

export async function loadDebug(filename: string): Promise<Debug | undefined> {
    const response = await fetch(filename);
    if (!response.ok) {
        console.error("Failed to load file:", response.statusText);
        return undefined;
    }
    const buffer = new Uint8Array(await response.arrayBuffer());
    const byteBuffer = new flatbuffers.ByteBuffer(buffer);
    return Debug.getRootAsDebug(byteBuffer);
}

import {AbstractLoader} from "$lib/loader/loader";
import {Interval, Id, Rectangle, Box, Edge} from "$lib/types";
import {Vector2, Vector3} from "three";

function parseInterval(interval): Interval {
    return new Interval(
        interval.min(),
        interval.max(),
        interval.mid(),
        interval.len()
    );
}

function parseId(id): Id {
    return new Id(
        id.depth(),
        id.bits(),
        parseInterval(id.interval())
    );
}

function parseVector(vector): Vector3 {
    return new Vector3(
        vector.x(),
        vector.y(),
        vector.z()
    );
}

function parseVertex(vertex): Vector3 {
    return new Vector3(
        vertex.x(),
        vertex.y(),
        vertex.z()
    );
}

export class DebugLoader extends AbstractLoader {
    public data;

    async load(path: string): Promise<void> {
        this.data = await loadDebug(path);
    }

    getBoxes(): Box[] {
        let boxes = [];
        if (this.data === undefined) {
            return boxes;
        }
        for (let boxIndex = 0; boxIndex < this.data!.boxesLength(); boxIndex++) {
            const box = this.data.boxes(boxIndex);
            boxes.push(new Box(
                parseId(box.theta()),
                parseId(box.phi()),
                parseId(box.alpha()),
                box.terminal()
            ));
        }
        return boxes;
    }

    getRectangles(boxIndex: number): Rectangle[] {
        let rectangles = [];
        if (this.data === undefined) {
            return rectangles;
        }
        let box = this.data.boxes(boxIndex);
        for (let rectangleIndex = 0; rectangleIndex < box.rectanglesLength(); rectangleIndex++) {
            const rectangle = box.rectangles(rectangleIndex);
            rectangles.push(new Rectangle(
                parseId(rectangle.theta()),
                parseId(rectangle.phi()),
                rectangle.terminal()
            ));
        }
        return rectangles;
    }

    getPlug(): Vector3[] {
        let plug = [];
        if (this.data === undefined) {
            return plug;
        }
        for (let vertexIndex = 0; vertexIndex < this.data.plug().verticesLength(); vertexIndex++) {
            let plugVertex = this.data.plug().vertices(vertexIndex);
            plug.push(new Vector3(plugVertex.x(), plugVertex.y(), plugVertex.z()));
        }
        return plug;
    }

    getHole(): Vector3[] {
        let hole = [];
        if (this.data === undefined) {
            return hole;
        }
        for (let vertexIndex = 0; vertexIndex < this.data.hole().verticesLength(); vertexIndex++) {
            let holeVertex = this.data.hole().vertices(vertexIndex);
            hole.push(new Vector3(holeVertex.x(), holeVertex.y(), holeVertex.z()));
        }
        return hole;
    }

    getHoleProjectionHull(boxIndex: number): Edge[] {
        let holeProjectionHull = [];
        if (this.data === undefined) {
            return holeProjectionHull;
        }
        let box = this.data.boxes(boxIndex);
        for (let edgeIndex = 0; edgeIndex < box.projection().edgesLength(); edgeIndex++) {
            let edge = box.projection.edges(edgeIndex);
            holeProjectionHull.push(new Edge(
                new Vector2(edge.start().x(), edge.start().y()),
                new Vector2(edge.end().x(), edge.end().y())
            ));
        }
        return holeProjectionHull;
    }

    getHoleInIndex(boxIndex: number): number {
        if (this.data === undefined) {
            return -1;
        }
        let box = this.data.boxes(boxIndex);
        return box.inIndex();
    }

    getHoleVertexProjections(boxIndex: number): Vector3[][] {
        let holeVertexProjections = [];
        if (this.data === undefined) {
            return holeVertexProjections;
        }
        let box = this.data.boxes(boxIndex);
        for (let vertexIndex = 0; vertexIndex < box.projection().verticesLength(); vertexIndex++) {
            let vertex = box.projection.vertices(vertexIndex);
            let projections = [];
            for (let projectionIndex = 0; projectionIndex < vertex.projectionsLength(); projectionIndex++) {
                let projection = vertex.projections(projectionIndex);
                projections.push(new Vector3(projection.x(), projection.y(), projection.z()));
            }
            holeVertexProjections.push(projections);
        }
        return holeVertexProjections;
    }

    getPlugVertexProjections(boxIndex: number): Vector3[][] {
        let plugVertexProjections = [];
        if (this.data === undefined) {
            return plugVertexProjections;
        }
        let box = this.data.boxes(boxIndex);
        for (let vertexIndex = 0; vertexIndex < box.projection().verticesLength(); vertexIndex++) {
            let vertex = box.projection.vertices(vertexIndex);
            let projections = [];
            for (let projectionIndex = 0; projectionIndex < vertex.projectionsLength(); projectionIndex++) {
                let projection = vertex.projections(projectionIndex);
                projections.push(new Vector3(projection.x(), projection.y(), projection.z()));
            }
            plugVertexProjections.push(projections);
        }
        return plugVertexProjections;
    }

    getPlugOutIndices(boxIndex: number, rectangleIndex: number): number[] {
        let outIndices = [];
        if (this.data === undefined) {
            return outIndices;
        }
        let box = this.data.boxes(boxIndex);
        let rectangle = box.rectangles(rectangleIndex);
        for (let outIndex = 0; outIndex < rectangle.outIndicesLength(); outIndex++) {
            outIndices.push(rectangle.outIndices(outIndex));
        }
        return outIndices;
    }
}