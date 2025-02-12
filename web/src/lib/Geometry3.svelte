<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";

    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {Mesh, MeshBasicMaterial, PerspectiveCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments, Vector3} from "three";
    import {ConvexGeometry} from 'three/addons/geometries/ConvexGeometry.js';
    import {AxesHelper} from "three";

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

    let camera: PerspectiveCamera;
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

    function setup(width: number, height: number) {
        {
            scene = new Scene();
            scene.up.set(0, 0, 1);
        }

        {
            camera = new PerspectiveCamera(60, width / height, 0.1, 1000);
            camera.up.set(0, 0, 2);
            camera.lookAt(0, 0, 2);
            camera.position.set(0, -8, 2);
        }

        {
            renderer = new WebGLRenderer({antialias: true});
            renderer.setSize(width, height);
        }

        {
            const controls = new OrbitControls(camera, renderer.domElement);
            controls.enableRotate = true;
            controls.enableZoom = true;
            controls.enablePan = false;
            controls.enableDamping = false;
            controls.target.set(0, 0, 2);
            controls.update();
        }

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        const n = 100;
        let points: Vector3[] = [];
        for (let i = 0; i < n; i++) {
            points.push(new Vector3(
                Math.random() * 2 - 1,
                Math.random() * 2 - 1,
                (Math.random() * 2 - 1) / 2 + 2
            ));
        }

        const polyhedron = new ConvexGeometry(points);
        const polyhedronMaterial = new MeshBasicMaterial({color: 0x00ffff, transparent: true, opacity: 0.25});
        const polyhedronMesh = new Mesh(polyhedron, polyhedronMaterial);
        scene.add(polyhedronMesh);

        const polyhedronEdges = new EdgesGeometry(polyhedron);
        const polyhedronEdgesMaterial = new LineBasicMaterial({color: 0x00ffff});
        const polyhedronEdgesMesh = new LineSegments(polyhedronEdges, polyhedronEdgesMaterial);
        scene.add(polyhedronEdgesMesh);

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number) {
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
    }
</script>

<ThreeElement {setup} {resize} {draw}/>
