<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flatbuffers_generated";
    import {Selection} from "$lib/state.svelte";

    import {MapControls} from 'three/addons/controls/MapControls.js';
    import {PlaneGeometry, Mesh, MeshBasicMaterial, OrthographicCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments, AxesHelper, Group} from "three";

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
    let box2s: Map<number, Group> = new Map();

    function onCover() {
        if (!cover) {
            return;
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        {
            let box3 = cover!.box3s(selection.selectedBox3)
            for (let index = 0; index < box3.box2sLength(); index++) {
                const box2 = box3.box2s(index);

                const phi = box2.phi().interval();
                const theta = box2.theta().interval();

                const box2Geometry = new PlaneGeometry((phi.max() - phi.min()) / (2 * Math.PI), (theta.max() - theta.min()) / Math.PI);
                const box2Material = new MeshBasicMaterial({color: 0x0000ff, transparent: true, opacity: 0.5});
                const box2Mesh = new Mesh(box2Geometry, box2Material);
                box2Mesh.position.set((phi.min() + phi.max()) / (2 * Math.PI) / 2, (theta.min() + theta.max()) / Math.PI / 2, 0);

                const box2EdgesGeometry = new EdgesGeometry(box2Geometry);
                const box2EdgesMaterial = new LineBasicMaterial({color: 0x00ffff});
                const box2Edges = new LineSegments(box2EdgesGeometry, box2EdgesMaterial);
                box2Edges.position.set((phi.min() + phi.max()) / (2 * Math.PI) / 2, (theta.min() + theta.max()) / Math.PI / 2, 0);

                const box2Group = new Group();
                box2Group.add(box2Mesh);
                box2Group.add(box2Edges);

                box2s.set(index, box2Group);
            }
        }
        {
            for (const box2 of box2s.values()) {
                scene.add(box2);
            }
        }
        return () => {
        };
    }

    function onSelectBox2() {
        if (selection.selectedBox2 === null) {
            return;
        }
        return () => {
        };
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
