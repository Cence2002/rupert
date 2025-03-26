import type {Vector2, Vector3} from "three";
import type {Box, Edge, Rectangle} from "$lib/Types";

export abstract class AbstractLoader {
    abstract load(directory: string): Promise<void>;

    // Boxes plot
    abstract getBox(boxIndex: number): Box | null;

    abstract getBoxes(): Box[];

    // Rectangles plot
    abstract getRectangle(boxIndex: number, rectangleIndex: number): Rectangle | null;

    abstract getRectangles(boxIndex: number): Rectangle[];

    // Main plot
    abstract getHole(): Vector3[];

    abstract getPlug(): Vector3[];

    // Projection plot
    abstract getHoleProjection(boxIndex: number): Edge[];

    abstract getHoleInIndex(boxIndex: number): number;

    abstract getHoleVertexProjections(boxIndex: number): Vector2[][];

    abstract getPlugVertexProjections(boxIndex: number, rectangleIndex: number): Vector2[][];

    abstract getPlugOutIndices(boxIndex: number, rectangleIndex: number): number[];
}