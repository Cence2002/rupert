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

function parseVector(vector): Vector2 {
    return new Vector2(
        vector.x(),
        vector.y(),
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

    getBox(boxIndex: number): Box {
        if (this.data === undefined) {
            return;
        }
        const box = this.data.boxes(boxIndex);
        return new Box(
            parseId(box.theta()),
            parseId(box.phi()),
            parseId(box.alpha()),
            box.terminal()
        );
    }

    getBoxes(): Box[] {
        let boxes = [];
        if (this.data === undefined) {
            return boxes;
        }
        for (let boxIndex = 0; boxIndex < this.data!.boxesLength(); boxIndex++) {
            boxes.push(this.getBox(boxIndex));
        }
        return boxes;
    }

    getRectangle(boxIndex: number, rectangleIndex: number): Rectangle {
        if (this.data === undefined) {
            return;
        }
        const box = this.data.boxes(boxIndex);
        const rectangle = box.rectangles(rectangleIndex);
        return new Rectangle(
            parseId(rectangle.theta()),
            parseId(rectangle.phi()),
            rectangle.outIndicesLength() > 0
        );
    }

    getRectangles(boxIndex: number): Rectangle[] {
        let rectangles = [];
        if (this.data === undefined) {
            return rectangles;
        }
        let box = this.data.boxes(boxIndex);
        for (let rectangleIndex = 0; rectangleIndex < box.rectanglesLength(); rectangleIndex++) {
            rectangles.push(this.getRectangle(boxIndex, rectangleIndex));
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
            plug.push(parseVertex(plugVertex));
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
            hole.push(parseVertex(holeVertex));
        }
        return hole;
    }

    getHoleInIndex(boxIndex: number): number {
        if (this.data === undefined) {
            return -1;
        }
        let box = this.data.boxes(boxIndex);
        return box.inIndex();
    }

    getHoleProjectionHull(boxIndex: number): Edge[] {
        let holeProjectionHull = [];
        if (this.data === undefined) {
            return holeProjectionHull;
        }
        let box = this.data.boxes(boxIndex);
        for (let edgeIndex = 0; edgeIndex < box.projection().edgesLength(); edgeIndex++) {
            let edge = box.projection().edges(edgeIndex);
            holeProjectionHull.push(new Edge(
                parseVector(edge.from()),
                parseVector(edge.to())
            ));
        }
        return holeProjectionHull;
    }

    getHoleVertexProjections(boxIndex: number): Vector2[][] {
        let holeVertexProjections = [];
        if (this.data === undefined) {
            return holeVertexProjections;
        }
        let box = this.data.boxes(boxIndex);
        for (let projectionIndex = 0; projectionIndex < box.projectionsLength(); projectionIndex++) {
            let projection = box.projections(projectionIndex);
            let vertexProjections = [];
            for (let vectorIndex = 0; vectorIndex < projection.vectorsLength(); vectorIndex++) {
                let vector = projection.vectors(vectorIndex);
                vertexProjections.push(parseVector(vector));
            }
            holeVertexProjections.push(vertexProjections);
        }
        return holeVertexProjections;
    }

    getPlugVertexProjections(boxIndex: number, rectangleIndex: number): Vector2[][] {
        let plugVertexProjections = [];
        if (this.data === undefined) {
            return plugVertexProjections;
        }
        let box = this.data.boxes(boxIndex);
        let rectangle = box.rectangles(rectangleIndex);
        for (let projectionIndex = 0; projectionIndex < rectangle.projectionsLength(); projectionIndex++) {
            let projection = rectangle.projections(projectionIndex);
            let vertexProjections = [];
            for (let vectorIndex = 0; vectorIndex < projection.vectorsLength(); vectorIndex++) {
                let vector = projection.vectors(vectorIndex);
                vertexProjections.push(parseVector(vector));
            }
            plugVertexProjections.push(vertexProjections);
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