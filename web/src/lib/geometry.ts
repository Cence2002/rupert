import {Vector2} from "three";


export const PI = Math.PI;
export const TWO_PI = 2 * Math.PI;

export function convexHull(points: Vector2[]): Vector2[] {
    if (points.length < 3) return points;

    let hull: Vector2[] = [];
    let leftmost: Vector2 = points.reduce((left, p) => (p.x < left.x ? p : left), points[0]!);
    let current: Vector2 = leftmost;

    do {
        hull.push(current);
        let next = points[0] === current ? points[1]! : points[0]!;

        for (let i = 0; i < points.length; i++) {
            let point = points[i]!;
            let cross = (next.x - current.x) * (point.y - current.y) - (next.y - current.y) * (point.x - current.x);
            if (cross < 0 || (cross === 0 && current.distanceTo(point) > current.distanceTo(next))) {
                next = point;
            }
        }

        current = next;
    } while (current !== leftmost);

    return hull;
}
