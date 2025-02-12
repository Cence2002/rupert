<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flatbuffers_generated";

    import {MapControls} from 'three/addons/controls/MapControls.js';
    import {PlaneGeometry, Mesh, MeshBasicMaterial, OrthographicCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments, AxesHelper} from "three";

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
        camera.left = (1 - zoom * aspect) / 2;
        camera.right = (1 + zoom * aspect) / 2;
        camera.top = (1 + zoom) / 2;
        camera.bottom = (1 - zoom) / 2;
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
            camera.lookAt(0.5, 0.5, 0);
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

        {
            const domainGeometry = new PlaneGeometry(1, 1);
            const domainEdgesGeometry = new EdgesGeometry(domainGeometry);
            const domainEdgeMaterial = new LineBasicMaterial({color: 0x7f7f7f});
            const domainEdges = new LineSegments(domainEdgesGeometry, domainEdgeMaterial);
            domainEdges.position.set(0.5, 0.5, 0);
            scene.add(domainEdges);
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
</script>

<ThreeElement {setup} {resize} {draw}/>
