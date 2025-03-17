import type {Vector3} from "three";
import type {Box, Edge, Rectangle} from "$lib/types";

export abstract class Loader {
    constructor(public path: string) {
    }

    abstract load(): void;

    // Boxes plot
    abstract getBoxes(): Box[];

    // Rectangles plot
    abstract getRectangles(boxIndex: number): Rectangle[];

    // Main plot
    abstract getHole(): Vector3[];

    abstract getPlug(): Vector3[];

    // Projection plot
    abstract getHoleProjectionHull(boxIndex: number): Edge[];

    abstract getHoleInIndex(boxIndex: number): number;

    abstract getHoleVertexProjections(boxIndex: number): Vector3[][];

    abstract getPlugVertexProjections(boxIndex: number): Vector3[][];

    abstract getPlugOutIndices(boxIndex: number): number[];
}