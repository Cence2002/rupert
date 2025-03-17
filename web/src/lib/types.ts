import {Vector2} from "three";

export class Interval {
    constructor(
        public min: number,
        public max: number
    ) {
    }
}

export class Id {
    constructor(
        public depth: number,
        public bits: number,
        public interval: Interval
    ) {
    }
}

export class Rectangle {
    constructor(
        public theta: Id,
        public phi: Id,
        public terminal: boolean
    ) {
    }
}

export class Box {
    constructor(
        public theta: Id,
        public phi: Id,
        public alpha: Id,
        public terminal: boolean
    ) {
    }
}

export class Edge {
    constructor(
        public start: Vector2,
        public end: Vector2
    ) {
    }
}