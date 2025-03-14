import * as flatbuffers from 'flatbuffers';

import {Boxes} from '$lib/flatbuffers/flatbuffers_generated';

export async function loadBoxes(file: string): Promise<Boxes | undefined> {
    const response = await fetch(file);
    if (!response.ok) {
        console.error("Failed to load file:", response.statusText);
        return undefined;
    }
    const buffer = new Uint8Array(await response.arrayBuffer());
    const byteBuffer = new flatbuffers.ByteBuffer(buffer);
    return Boxes.getRootAsBoxes(byteBuffer);
}
