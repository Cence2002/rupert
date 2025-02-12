<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";

    import {MapControls} from "three/addons/controls/MapControls.js";
    import {Scene, WebGLRenderer, OrthographicCamera, Vector2, Shape, ShapeGeometry, MeshBasicMaterial, Mesh, EdgesGeometry, LineBasicMaterial, LineSegments, AxesHelper} from "three";

    let {cover, selectedBox3, selectedBox2} = $props<{
        cover: Cover | undefined,
        selectedBox3: number | null,
        selectedBox2: number | null,
    }>();

    $effect(() => {
        if (cover) {
            processCover();
        }
    });

    $effect(() => {
        createBox3Selection();

        return () => {
            clearBox3Selection();
        };
    });

    $effect(() => {
        createBox2Selection();

        return () => {
            clearBox2Selection();
        };
    });

    let camera: OrthographicCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;

    function processCover() {
    }

    function createBox3Selection() {
    }

    function clearBox3Selection() {
    }

    function createBox2Selection() {
    }

    function clearBox2Selection() {
    }

    function setCameraBounds(width: number, height: number, zoom: number = 2) {
        const aspect = width / height;
        camera.left = -zoom * aspect / 2;
        camera.right = zoom * aspect / 2;
        camera.top = zoom / 2;
        camera.bottom = -zoom / 2;
        camera.updateProjectionMatrix();
    }

    function setup(width: number, height: number) {
        // scene = new Scene();
        //
        // const aspect = width / height;
        // camera = new OrthographicCamera(
        //     -aspect, aspect,
        //     1, -1,
        //     -1, 1
        // );
        // camera.position.set(0, 0, 1);
        // camera.lookAt(0, 0, 0);
        //
        // renderer = new WebGLRenderer({antialias: true});
        // renderer.setSize(width, height);
        //
        // // Generate random points
        // let points: Vector2[] = [];
        // for (let i = 0; i < 20; i++) {
        //     points.push(new Vector2(Math.random() * 2 - 1, Math.random() * 2 - 1));
        // }
        //
        // // Compute and display the convex hull
        // if (points.length > 2) {
        //     addConvexHull(points);
        // }
        //
        // // Controls
        // const controls = new MapControls(camera, renderer.domElement);
        // controls.enableRotate = false;
        // controls.enableZoom = true;
        // controls.enablePan = true;
        // controls.screenSpacePanning = true;
        // controls.zoomToCursor = true;
        //
        // return renderer.domElement;

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

        const n = 100;
        let points: Vector2[] = [];
        for (let i = 0; i < n; i++) {
            points.push(new Vector2(
                (Math.random() * 2 - 1) / 4,
                (Math.random() * 2 - 1) / 4
            ));
        }

        if (points.length > 2) {
            addConvexHull(points);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number) {
        const aspect = width / height;
        camera.left = -aspect;
        camera.right = aspect;
        camera.top = 1;
        camera.bottom = -1;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
    }

    function addConvexHull(points: Vector2[]) {
        // Compute convex hull using Graham scan
        const hull = computeConvexHull(points);
        if (hull.length < 3) return; // Must have at least 3 points

        // Create filled polygon
        const shape = new Shape(hull);
        const geometry = new ShapeGeometry(shape);
        const material = new MeshBasicMaterial({color: 0x00ffff, transparent: true, opacity: 0.2, side: 2});
        const mesh = new Mesh(geometry, material);
        scene.add(mesh);

        // Draw hull edges
        const edges = new EdgesGeometry(geometry);
        const edgeMaterial = new LineBasicMaterial({color: 0x00ffff});
        const edgeLines = new LineSegments(edges, edgeMaterial);
        scene.add(edgeLines);

        // Draw points
        const pointMaterial = new MeshBasicMaterial({color: 0x00ffff});
        for (let p of points) {
            const pointGeometry = new ShapeGeometry(new Shape([p]));
            const pointMesh = new Mesh(pointGeometry, pointMaterial);
            pointMesh.position.set(p.x, p.y, 0);
            scene.add(pointMesh);
        }
    }

    function computeConvexHull(points: Vector2[]): Vector2[] {
        // Sort points lexicographically
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
