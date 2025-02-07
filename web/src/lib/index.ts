// place files you want to import through the `$lib` alias in this folder.

export interface RectSize {
    width: number;
    height: number;
}

export function assert(condition: any, msg?: string): asserts condition {
    if (!condition) {
        throw new Error(msg);
    }
}