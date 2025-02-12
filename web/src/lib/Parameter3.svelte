<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";

    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {BoxGeometry, Mesh, MeshBasicMaterial, PerspectiveCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments} from "three";
    import {AxesHelper} from "three";

    let {cover, selectedBox3, selectBox3} = $props<{
        cover: Cover | undefined,
        selectedBox3: number | null,
        selectBox3: (index: number) => void
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

    let camera: PerspectiveCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;

    function processCover() {
        {
            for (let index = 0; index < cover!.box3sLength(); index++) {
                const box3 = cover!.box3s(index);

                const phi = box3.phi().interval();
                const theta = box3.theta().interval();
                const alpha = box3.alpha().interval();

                const box3Geometry = new BoxGeometry((phi.max() - phi.min()) / (2 * Math.PI), (theta.max() - theta.min()) / Math.PI, (alpha.max() - alpha.min()) / (2 * Math.PI));
                const box3Material = new MeshBasicMaterial({color: 0x0000ff, transparent: true, opacity: 0.25});
                const box3Mesh = new Mesh(box3Geometry, box3Material);
                box3Mesh.position.set((phi.min() + phi.max()) / (2 * Math.PI) / 2, (theta.min() + theta.max()) / Math.PI / 2, (alpha.min() + alpha.max()) / (2 * Math.PI) / 2);
                scene.add(box3Mesh);

                const box3EdgesGeometry = new EdgesGeometry(box3Geometry);
                const box3EdgesMaterial = new LineBasicMaterial({color: 0x00ffff});
                const box3Edges = new LineSegments(box3EdgesGeometry, box3EdgesMaterial);
                box3Edges.position.set((phi.min() + phi.max()) / (2 * Math.PI) / 2, (theta.min() + theta.max()) / Math.PI / 2, (alpha.min() + alpha.max()) / (2 * Math.PI) / 2);
                scene.add(box3Edges);
            }
        }
    }

    function createBox3Selection() {
    }

    function clearBox3Selection() {
    }

    function setup(width: number, height: number) {
        {
            scene = new Scene();
            scene.up.set(0, 0, 1);
        }

        {
            camera = new PerspectiveCamera(60, width / height, 0.1, 1000);
            camera.up.set(0, 0, 1);
            camera.lookAt(0.5, 0.5, 0.5);
            camera.position.set(0.5, -1.5, 0.5);
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
            controls.target.set(0.5, 0.5, 0.5);
            controls.update();
        }

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        {
            const domainGeometry = new BoxGeometry(1, 1, 1);
            const domainEdgesGeometry = new EdgesGeometry(domainGeometry);
            const domainEdgeMaterial = new LineBasicMaterial({color: 0x7f7f7f});
            const domainEdges = new LineSegments(domainEdgesGeometry, domainEdgeMaterial);
            domainEdges.position.set(0.5, 0.5, 0.5);
            scene.add(domainEdges);
        }

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
