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
