import * as flatbuffers from 'flatbuffers';

import {Data} from '$lib/flatbuffers/flatbuffers_generated';

export async function loadData(file: string): Promise<Data | undefined> {
    const response = await fetch(file);
    if (!response.ok) {
        console.error("Failed to load file:", response.statusText);
        return;
    }
    const buffer = new Uint8Array(await response.arrayBuffer());
    const byteBuffer = new flatbuffers.ByteBuffer(buffer);
    return Data.getRootAsData(byteBuffer);
}