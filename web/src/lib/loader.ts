import * as flatbuffers from 'flatbuffers';

import {Cover} from '$lib/flatbuffers/flatbuffers_generated';

export async function loadCover(file: string): Promise<Cover | undefined> {
    const response = await fetch(file);
    if (!response.ok) {
        console.error("Failed to load file:", response.statusText);
        return undefined;
    }
    // await new Promise(resolve => setTimeout(resolve, 1000));
    const buffer = new Uint8Array(await response.arrayBuffer());
    const byteBuffer = new flatbuffers.ByteBuffer(buffer);
    return Cover.getRootAsCover(byteBuffer);
}
