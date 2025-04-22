import type {Vector2, Vector3} from "three";
import type {Box, Edge, Rectangle} from "$lib/Types";

export interface AbstractLoader {
    // Load data

    load(directory: string): Promise<void>;

    // Boxes plot

    getBox(boxIndex: number): Box | null;

    getBoxes(): Box[];

    getBoxCount(): number;

    getNonterminalBox(boxIndex: number): Box | null;

    getNonterminalBoxes(): Box[];

    getNonterminalBoxCount(): number;

    // Rectangles plot

    getRectangle(boxIndex: number, rectangleIndex: number): Rectangle | null;

    getRectangles(boxIndex: number): Rectangle[];

    getRectangleCount(boxIndex: number): number;

    // Main plot

    getHole(): Vector3[];

    getPlug(): Vector3[];

    // Projection plot

    getHoleProjection(boxIndex: number): Edge[];

    getHoleVertexProjections(boxIndex: number): Vector2[][];

    getInRectangleIndex(boxIndex: number): number | null;

    getPlugVertexProjections(boxIndex: number, rectangleIndex: number): Vector2[][];

    getOutVertexIndices(boxIndex: number, rectangleIndex: number): number[];
}
