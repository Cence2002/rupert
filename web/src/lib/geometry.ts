import {Vector2} from "three";


export function convexHull(points: Vector2[]): Vector2[] {
    points.sort((a, b) => a.x - b.x || a.y - b.y);

    const cross = (o: Vector2, a: Vector2, b: Vector2) =>
        (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);

    let lower: Vector2[] = [];
    for (let p of points) {
        while (lower.length >= 2 && cross(lower[lower.length - 2], lower[lower.length - 1], p) <= 0) {
            lower.pop();
        }
        lower.push(p);
    }

    let upper: Vector2[] = [];
    for (let i = points.length - 1; i >= 0; i--) {
        let p = points[i];
        while (upper.length >= 2 && cross(upper[upper.length - 2], upper[upper.length - 1], p) <= 0) {
            upper.pop();
        }
        upper.push(p);
    }

    upper.pop();
    lower.pop();
    return lower.concat(upper);
}
