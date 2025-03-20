import {Vector2} from "three";

export const PI = Math.PI;
export const TWO_PI = 2 * Math.PI;

export function convexHull(points: Vector2[]): Vector2[] {
    if (points.length < 3) return points;

    const hull: Vector2[] = [];
    const leftmost: Vector2 = points.reduce((left, p) => (p.x < left.x ? p : left), points[0]!);
    let current: Vector2 = leftmost;

    do {
        hull.push(current);
        let next = points[0] === current ? points[1]! : points[0]!;

        for (const point of points) {
            const cross = new Vector2().subVectors(next, current).cross(new Vector2().subVectors(point, current));
            if (cross < 0 || (cross === 0 && current.distanceTo(point) > current.distanceTo(next))) {
                next = point;
            }
        }

        current = next;
    } while (current !== leftmost);

    return hull;
}
