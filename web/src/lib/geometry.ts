import {Vector2} from "three";


export function convexHull(points: Vector2[]): Vector2[] {
    if (points.length < 3) return points;

    let hull: Vector2[] = [];
    let leftmost = points.reduce((left, p) => (p.x < left.x ? p : left), points[0]);
    let current = leftmost;

    do {
        hull.push(current);
        let next = points[0] === current ? points[1] : points[0];

        for (let i = 0; i < points.length; i++) {
            let cross = (next.x - current.x) * (points[i].y - current.y) - (next.y - current.y) * (points[i].x - current.x);
            if (cross < 0 || (cross === 0 && current.distanceTo(points[i]) > current.distanceTo(next))) {
                next = points[i];
            }
        }

        current = next;
    } while (current !== leftmost);

    return hull;
}
