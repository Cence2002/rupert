import {AbstractLoader} from "$lib/loader/AbstractLoader";
import {Vector2, Vector3} from "three";
import {Interval, Id, Rectangle, Box, Edge, TerminalBox} from "$lib/Types";
import {PI, TWO_PI} from "$lib/Geometry";

class BinaryReader {
    private dataView: DataView;
    private offset: number;

    constructor(buffer: ArrayBuffer) {
        this.dataView = new DataView(buffer);
        this.offset = 0;
    }

    get remainingBytes(): number {
        return this.dataView.byteLength - this.offset;
    }

    get isEOF(): boolean {
        return this.offset >= this.dataView.byteLength;
    }

    readUint32(): number {
        if (this.remainingBytes < 4) {
            throw new Error("Not enough bytes to read uint32.");
        }
        const value = this.dataView.getUint32(this.offset, true);
        this.offset += 4;
        return value;
    }

    readUint16(): number {
        if (this.remainingBytes < 2) {
            throw new Error("Not enough bytes to read uint16.");
        }
        const value = this.dataView.getUint16(this.offset, true);
        this.offset += 2;
        return value;
    }

    readFloat64(): number {
        if (this.remainingBytes < 8) {
            throw new Error("Not enough bytes to read float64.");
        }
        const value = this.dataView.getFloat64(this.offset, true);
        this.offset += 8;
        return value;
    }
}

function parsePolyhedron(reader: BinaryReader): Vector3[] {
    const vertexCount = reader.readUint32();
    const vertices: Vector3[] = [];

    for (let i = 0; i < vertexCount; i++) {
        const x = reader.readFloat64();
        const y = reader.readFloat64();
        const z = reader.readFloat64();
        vertices.push(new Vector3(x, y, z));
    }

    return vertices;
}

function parseId(reader: BinaryReader, scale: number = 1): Id {
    const packed = reader.readUint16();
    let depth: number, bits: number;
    if (packed === 0) {
        depth = 16;
        bits = 0;
    } else {
        const leadingZeros = Math.clz32(packed);
        depth = 31 - leadingZeros;
        bits = packed & ((1 << depth) - 1);
    }
    const min = bits / (1 << depth) * scale;
    const max = (bits + 1) / (1 << depth) * scale;
    const mid = (min + max) / 2;
    const len = max - min;
    const interval = new Interval(min, max, mid, len);
    return new Id(depth, bits, interval);
}

function parseBox(reader: BinaryReader): Box {
    const theta = parseId(reader, TWO_PI);
    const phi = parseId(reader, PI);
    const alpha = parseId(reader, TWO_PI);
    return new Box(theta, phi, alpha, true);
}

function parseRectangle(reader: BinaryReader): Rectangle {
    const theta = parseId(reader, TWO_PI);
    const phi = parseId(reader, PI);
    return new Rectangle(theta, phi, true);
}

function parseTerminalBox(reader: BinaryReader): TerminalBox {
    const box = parseBox(reader);
    const rectCount = reader.readUint32();
    const rectangles: Rectangle[] = [];

    for (let i = 0; i < rectCount; i++) {
        rectangles.push(parseRectangle(reader));
    }

    return new TerminalBox(box, rectangles);
}

export function parsePolyhedraFile(buffer: ArrayBuffer) {
    const reader = new BinaryReader(buffer);
    const hole = parsePolyhedron(reader);
    const plug = parsePolyhedron(reader);
    return {hole, plug};
}

export function parseAllTerminalBoxes(buffer: ArrayBuffer): TerminalBox[] {
    const reader = new BinaryReader(buffer);
    const result: TerminalBox[] = [];

    while (!reader.isEOF) {
        if (reader.remainingBytes < 14) {
            break;
        }
        const terminalBox = parseTerminalBox(reader);
        result.push(terminalBox);
        if (result.length % 100 === 0) {
            console.log("Parsed", result.length, "terminal boxes.");
        }
    }

    return result;
}

export class Loader extends AbstractLoader {
    public hole: Vector3[] = [];
    public plug: Vector3[] = [];
    public terminalBoxes: TerminalBox[] = [];

    async loadPolyhedra(path: string): Promise<void> {
        const response = await fetch(path);
        if (!response.ok) {
            console.error("Failed to load file:", response.statusText);
            return undefined;
        }
        const buffer = await response.arrayBuffer();
        const {hole, plug} = parsePolyhedraFile(buffer);
        this.hole = hole;
        this.plug = plug;
    }

    async load(path: string): Promise<void> {
        const response = await fetch(path);
        if (!response.ok) {
            console.error("Failed to load file:", response.statusText);
            return undefined;
        }
        const buffer = await response.arrayBuffer();
        this.terminalBoxes = parseAllTerminalBoxes(buffer);
    }

    getBox(boxIndex: number): Box {
        return this.terminalBoxes[boxIndex].box;
    }

    getBoxes(): Box[] {
        return this.terminalBoxes.map(terminalBox => terminalBox.box);
    }

    getRectangle(boxIndex: number, rectangleIndex: number): Rectangle {
        return this.terminalBoxes[boxIndex].rectangles[rectangleIndex];
    }

    getRectangles(boxIndex: number): Rectangle[] {
        return this.terminalBoxes[boxIndex].rectangles;
    }

    getPlug(): Vector3[] {
        return this.plug;
    }

    getHole(): Vector3[] {
        return this.hole;
    }


    getHoleProjection(boxIndex: number) {
        //TODO: Implement
        return [];
    }

    getHoleInIndex(boxIndex: number) {
        //TODO: Implement
        return 0;
    }

    getHoleVertexProjections(boxIndex: number) {
        //TODO: Implement
        return [];
    }

    getPlugVertexProjections(boxIndex: number, rectangleIndex: number) {
        //TODO: Implement
        return [];
    }

    getPlugOutIndices(boxIndex: number, holeIndex: number) {
        //TODO: Implement
        return [];
    }
}