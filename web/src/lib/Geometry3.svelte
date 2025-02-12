<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";

    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {Mesh, MeshBasicMaterial, PerspectiveCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments, Vector3} from "three";
    import {ConvexGeometry} from 'three/addons/geometries/ConvexGeometry.js';

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

    let camera: PerspectiveCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;

    function processCover() {
        console.log("processing in geometry3", cover.description());
    }

    $effect(() => {
        console.log(selectedBox3, selectedBox2, "in geometry3");

        return () => {
            console.log(selectedBox3, selectedBox2, "in geometry3 cleanup");
        };
    });

    function setup(width: number, height: number) {
        scene = new Scene();
        scene.up.set(0, 0, 1);
        camera = new PerspectiveCamera(60, width / height, 0.1, 1000);
        renderer = new WebGLRenderer({antialias: true});
        renderer.setSize(width, height);


        const n = 100;
        let points: Vector3[] = [];
        for (let i = 0; i < n; i++) {
            points.push(new Vector3(
                Math.random() * 2 - 1,
                Math.random() * 2 - 1,
                (Math.random() * 2 - 1) / 2
            ));
        }

        const polyhedron = new ConvexGeometry(points);
        const polyhedronMaterial = new MeshBasicMaterial({color: 0x00ffff, transparent: true, opacity: 0.25});
        const polyhedronMesh = new Mesh(polyhedron, polyhedronMaterial);
        scene.add(polyhedronMesh);

        const edges = new EdgesGeometry(polyhedron);
        const edgesMaterial = new LineBasicMaterial({color: 0x00ffff});
        const edgesMesh = new LineSegments(edges, edgesMaterial);
        scene.add(edgesMesh);

        camera.up.set(0, 0, 1);
        camera.position.set(0, 4, 0);

        const controls = new OrbitControls(camera, renderer.domElement);
        controls.enableRotate = true;
        controls.enableZoom = true;
        controls.enablePan = false;
        controls.enableDamping = false;

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
