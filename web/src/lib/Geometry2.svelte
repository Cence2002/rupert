<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";
    import {Selection} from "$lib/state.svelte";

    import {MapControls} from "three/addons/controls/MapControls.js";
    import {
        Scene,
        WebGLRenderer,
        OrthographicCamera,
        Vector2,
        Shape,
        ShapeGeometry,
        MeshBasicMaterial,
        Mesh,
        EdgesGeometry,
        LineBasicMaterial,
        LineSegments,
        AxesHelper,
        FrontSide,
        Group,
        Vector3,
        Line, BufferGeometry
    } from "three";

    let {cover, selection} = $props<{
        cover: Cover | undefined,
        selection: Selection,
    }>();

    $effect(onCover);

    $effect(onSelectBox3);

    $effect(onSelectBox2);

    let camera: OrthographicCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;
    let projectionLines: Line[] = [];
    let projections: Group[] = [];
    let box2Projections: Group[] = [];
    let box2Out: number[] = [];

    function onCover() {
        if (!cover) {
            return;
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        const box3 = cover!.box3s(selection.selectedBox3);

        for (let index = 0; index < box3.projectionsLength(); index++) {
            const projection = box3.projections(index);
            const vertices: Vector2[] = [];
            for (let index2 = 0; index2 < projection.verticesLength(); index2++) {
                const vertex = projection.vertices(index2);
                vertices.push(new Vector2(vertex.x(), vertex.y()));
            }

            const hull = new Shape(computeConvexHull(vertices));
            const hullGeometry = new ShapeGeometry(hull);
            const hullMaterial = new MeshBasicMaterial({
                color: 0x0000ff,
                transparent: true,
                opacity: 0.25,
                side: FrontSide,
                depthWrite: false
            });
            const hullMesh = new Mesh(hullGeometry, hullMaterial);

            const hullEdgesGeometry = new EdgesGeometry(hullGeometry);
            const hullEdgesMaterial = new LineBasicMaterial({color: 0x0000ff});
            const hullEdges = new LineSegments(hullEdgesGeometry, hullEdgesMaterial);

            const group = new Group();
            group.add(hullMesh);
            group.add(hullEdges);

            projections.push(group);
        }
        for (let group of projections) {
            scene.add(group);
        }

        const projection = box3.projection();
        for (let index = 0; index < projection.linesLength(); index++) {
            const line = projection.lines(index);
            const point = new Vector2(line.point().x(), line.point().y());
            const direction = new Vector2(line.direction().x(), line.direction().y());
            const lineGeometry = new BufferGeometry().setFromPoints([
                new Vector3(point.x, point.y, 0),
                new Vector3(point.x + direction.x, point.y + direction.y, 0)]);
            const lineMaterial = new LineBasicMaterial({color: 0x7f7f7f});
            const lineMesh = new Line(lineGeometry, lineMaterial);
            projectionLines.push(lineMesh);
        }

        for (let projectionLine of projectionLines) {
            scene.add(projectionLine);
        }

        return () => {
            for (let group of projections) {
                scene.remove(group);
            }
            projections = [];
            for (let arrow of projectionLines) {
                scene.remove(arrow);
            }
            projectionLines = [];
        };
    }

    function onSelectBox2() {
        if (selection.selectedBox2 === null) {
            return;
        }
        const box3 = cover!.box3s(selection.selectedBox3);
        const box2 = box3.box2s(selection.selectedBox2);

        for (let index = 0; index < box2.outLength(); index++) {
            box2Out.push(box2.out(index));
        }

        for (let index = 0; index < box2.projectionsLength(); index++) {
            const projection = box2.projections(index);
            const vertices: Vector2[] = [];
            for (let index2 = 0; index2 < projection.verticesLength(); index2++) {
                const vertex = projection.vertices(index2);
                vertices.push(new Vector2(vertex.x(), vertex.y()));
            }

            const hull = new Shape(computeConvexHull(vertices));
            const hullGeometry = new ShapeGeometry(hull);
            const hullMaterial = new MeshBasicMaterial({
                color: 0x00ff00,
                transparent: true,
                // TODO: fix so that it's the bo2's index that is checked
                // opacity: (index == box3.in_()) ? 0.25 : (box2Out.includes(index) ? 0.5 : 0),
                opacity: (box2Out.includes(index) ? 0.5 : 0),
                side: FrontSide,
                depthWrite: false,
            });
            const hullMesh = new Mesh(hullGeometry, hullMaterial);

            const hullEdgesGeometry = new EdgesGeometry(hullGeometry);
            const hullEdgesMaterial = new LineBasicMaterial({color: 0x7f7f7f});
            const hullEdges = new LineSegments(hullEdgesGeometry, hullEdgesMaterial);

            const group = new Group();
            group.add(hullMesh);
            group.add(hullEdges);

            box2Projections.push(group);
        }
        for (let group of box2Projections) {
            scene.add(group);
        }

        return () => {
            for (let group of box2Projections) {
                scene.remove(group);
            }
            box2Projections = [];
        };
    }

    function setCameraBounds(width: number, height: number, zoom: number = 8) {
        const aspect = width / height;
        camera.left = -zoom * aspect / 2;
        camera.right = zoom * aspect / 2;
        camera.top = zoom / 2;
        camera.bottom = -zoom / 2;
        camera.updateProjectionMatrix();
    }

    function setup(width: number, height: number) {
        {
            scene = new Scene();
            scene.up.set(0, 1, 0);
        }

        {
            camera = new OrthographicCamera(0, 0, 0, 0, -1, 1);
            setCameraBounds(width, height);
            camera.up.set(0, 1, 0);
            camera.lookAt(0, 0, 0);
        }

        {
            renderer = new WebGLRenderer({antialias: true});
            renderer.setSize(width, height);
        }

        {
            const controls = new MapControls(camera, renderer.domElement);
            controls.enablePan = true;
            controls.enableZoom = true;
            controls.enableRotate = false;
            controls.screenSpacePanning = true;
            controls.zoomToCursor = true;
            controls.update();
        }

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number) {
        setCameraBounds(width, height);
        renderer.setSize(width, height);
    }

    function computeConvexHull(points: Vector2[]): Vector2[] {
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
</script>

<ThreeElement {setup} {resize} {draw}/>
