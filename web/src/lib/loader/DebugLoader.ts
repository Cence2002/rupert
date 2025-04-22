import * as flatbuffers from 'flatbuffers';
import {Debug, Vector, Vertex} from '$lib/flatbuffers/flatbuffers_generated';
import type {AbstractLoader} from "$lib/loader/AbstractLoader";
import {Interval, Id, Rectangle, Box, Edge} from "$lib/Types";
import {Vector2, Vector3} from "three";

// @ts-ignore
function parseInterval(interval): Interval {
    return new Interval(
        interval.min(),
        interval.max(),
    );
}

// @ts-ignore
function parseId(id): Id {
    return new Id(
        id.depth(),
        id.bits(),
        parseInterval(id.interval())
    );
}

function parseVector(vector: Vector): Vector2 {
    return new Vector2(
        vector.x(),
        vector.y(),
    );
}

function parseVertex(vertex: Vertex): Vector3 {
    return new Vector3(
        vertex.x(),
        vertex.y(),
        vertex.z()
    );
}

export class DebugLoader implements AbstractLoader {
    public data: Debug | undefined;

    async loadDebug(path: string): Promise<void> {
        const response = await fetch(path);
        if (!response.ok) {
            console.error("Failed to load file:", response.statusText);
            return undefined;
        }
        const buffer = new Uint8Array(await response.arrayBuffer());
        const byteBuffer = new flatbuffers.ByteBuffer(buffer);
        this.data = Debug.getRootAsDebug(byteBuffer);
    }

    async load(directory: string): Promise<void> {
        await this.loadDebug(directory + "/debug.bin");
    }

    getBox(boxIndex: number): Box | null {
        const box = this.data!.boxes(boxIndex)!;
        return new Box(
            parseId(box.theta()),
            parseId(box.phi()),
            parseId(box.alpha()),
            box.terminal()
        );
    }

    getBoxes(): Box[] {
        const boxes: Box[] = [];
        for (let boxIndex = 0; boxIndex < this.data!.boxesLength(); boxIndex++) {
            boxes.push(this.getBox(boxIndex)!);
        }
        return boxes;
    }

    getBoxCount(): number {
        return this.data!.boxesLength();
    }

    getNonterminalBox(boxIndex: number): Box | null {
        return null;
    }

    getNonterminalBoxes(): Box[] {
        return [];
    }

    getNonterminalBoxCount(): number {
        return 0;
    }

    getRectangle(boxIndex: number, rectangleIndex: number): Rectangle | null {
        const box = this.data!.boxes(boxIndex)!;
        const rectangle = box.rectangles(rectangleIndex)!;
        return new Rectangle(
            parseId(rectangle.theta()),
            parseId(rectangle.phi()),
            rectangle.outIndicesLength() > 0
        );
    }

    getRectangles(boxIndex: number): Rectangle[] {
        const rectangles: Rectangle[] = [];
        const box = this.data!.boxes(boxIndex)!;
        for (let rectangleIndex = 0; rectangleIndex < box.rectanglesLength(); rectangleIndex++) {
            rectangles.push(this.getRectangle(boxIndex, rectangleIndex)!);
        }
        return rectangles;
    }

    getRectangleCount(boxIndex: number): number {
        const box = this.data!.boxes(boxIndex)!;
        return box.rectanglesLength();
    }

    getHole(): Vector3[] {
        const hole: Vector3[] = [];
        for (let vertexIndex = 0; vertexIndex < this.data!.hole()!.verticesLength(); vertexIndex++) {
            const holeVertex = this.data!.hole()!.vertices(vertexIndex)!;
            hole.push(parseVertex(holeVertex));
        }
        return hole;
    }

    getPlug(): Vector3[] {
        const plug: Vector3[] = [];
        for (let vertexIndex = 0; vertexIndex < this.data!.plug()!.verticesLength(); vertexIndex++) {
            const plugVertex = this.data!.plug()!.vertices(vertexIndex)!;
            plug.push(parseVertex(plugVertex));
        }
        return plug;
    }

    getHoleProjection(boxIndex: number): Edge[] {
        const holeProjectionHull: Edge[] = [];
        const box = this.data!.boxes(boxIndex)!;
        for (let edgeIndex = 0; edgeIndex < box.projection()!.edgesLength(); edgeIndex++) {
            const edge = box.projection()!.edges(edgeIndex)!;
            holeProjectionHull.push(new Edge(
                parseVector(edge.from()!),
                parseVector(edge.to()!)
            ));
        }
        return holeProjectionHull;
    }

    getInRectangleIndex(boxIndex: number): number | null {
        const box = this.data!.boxes(boxIndex)!;
        const inIndex = box.inIndex()
        return inIndex === -1 ? null : inIndex;
    }

    getHoleVertexProjections(boxIndex: number): Vector2[][] {
        const holeVertexProjections: Vector2[][] = [];
        const box = this.data!.boxes(boxIndex)!;
        for (let projectionIndex = 0; projectionIndex < box.projectionsLength(); projectionIndex++) {
            const projection = box.projections(projectionIndex)!;
            const vertexProjections = [];
            for (let vectorIndex = 0; vectorIndex < projection.vectorsLength(); vectorIndex++) {
                const vector = projection.vectors(vectorIndex)!;
                vertexProjections.push(parseVector(vector));
            }
            holeVertexProjections.push(vertexProjections);
        }
        return holeVertexProjections;
    }

    getPlugVertexProjections(boxIndex: number, rectangleIndex: number): Vector2[][] {
        const plugVertexProjections: Vector2[][] = [];
        const box = this.data!.boxes(boxIndex)!;
        const rectangle = box.rectangles(rectangleIndex)!;
        for (let projectionIndex = 0; projectionIndex < rectangle.projectionsLength(); projectionIndex++) {
            const projection = rectangle.projections(projectionIndex)!;
            const vertexProjections = [];
            for (let vectorIndex = 0; vectorIndex < projection.vectorsLength(); vectorIndex++) {
                const vector = projection.vectors(vectorIndex)!;
                vertexProjections.push(parseVector(vector));
            }
            plugVertexProjections.push(vertexProjections);
        }
        return plugVertexProjections;
    }

    getOutVertexIndices(boxIndex: number, rectangleIndex: number): number[] {
        const outIndices: number[] = [];
        const box = this.data!.boxes(boxIndex)!;
        const rectangle = box.rectangles(rectangleIndex)!;
        for (let outIndex = 0; outIndex < rectangle.outIndicesLength(); outIndex++) {
            outIndices.push(rectangle.outIndices(outIndex)!);
        }
        return outIndices;
    }
}