<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flatbuffers_generated";

    import {MapControls} from 'three/addons/controls/MapControls.js';
    import {PlaneGeometry, Mesh, MeshBasicMaterial, OrthographicCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments} from "three";

    let {cover, selectedBox3, selectedBox2, selectBox2} = $props<{
        cover: Cover | undefined,
        selectedBox3: number | null,
        selectedBox2: number | null,
        selectBox2: (index: number) => void
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
    let squares: Mesh[] = [];

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
        scene = new Scene();

        const aspect = width / height;
        camera = new OrthographicCamera(
            -aspect, aspect,
            1, -1,
            -1, 1
        );
        camera.position.set(0, 0, 1);
        camera.lookAt(0, 0, 0);

        renderer = new WebGLRenderer({antialias: true});
        renderer.setSize(width, height);

        const material = new MeshBasicMaterial({color: 0x00ffff, transparent: true, opacity: 0.7});

        const n = 20;
        for (let x = 0; x < n; x++) {
            for (let y = 0; y < n; y++) {
                const geometry = new PlaneGeometry(2 / n, 2 / n);
                const square = new Mesh(geometry, material);
                square.position.set(
                    x / n * 2 - 1 + 1 / n,
                    y / n * 2 - 1 + 1 / n,
                    0
                );

                if (square.position.length() > 0.9 || square.position.x ** 2 < 0.2) {
                    continue;
                }

                scene.add(square);
                squares.push(square);
            }
        }

        const frameGeometry = new PlaneGeometry(2, 2);
        const edges = new EdgesGeometry(frameGeometry);
        const edgeMaterial = new LineBasicMaterial({color: 0xff0000});
        const squareFrame = new LineSegments(edges, edgeMaterial);
        scene.add(squareFrame);

        const controls = new MapControls(camera, renderer.domElement);
        controls.enableRotate = false;
        controls.enableZoom = true;
        controls.enablePan = true;
        controls.screenSpacePanning = true;
        controls.zoomToCursor = true;

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
        if (height > width) selectBox2(1);
    }
</script>

<ThreeElement {setup} {resize} {draw}/>
