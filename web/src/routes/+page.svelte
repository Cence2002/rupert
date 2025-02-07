<script lang="ts">
    import ThreeEmbed from "$lib/ThreeEmbed.svelte";
    import type {RectSize} from "$lib";
    import {BoxGeometry, Mesh, MeshBasicMaterial, PerspectiveCamera, Scene, WebGLRenderer} from "three";
    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';

    let camera: PerspectiveCamera | null = $state(null);
    let scene: Scene | null = $state(null);
    let renderer: WebGLRenderer | null = $state(null);
    let cube: Mesh | null = $state(null);

    function setup(size: RectSize) {
        scene = new Scene();
        camera = new PerspectiveCamera(75, size.width / size.height, 0.1, 1000);
        renderer = new WebGLRenderer();
        renderer.setSize(size.width, size.height);

        const geometry = new BoxGeometry();
        const material = new MeshBasicMaterial({color: 0x00ffff});
        cube = new Mesh(geometry, material);
        scene.add(cube);

        camera.position.z = 5;

        const controls = new OrbitControls(camera, renderer.domElement);
        controls.enableRotate = false;
        controls.enablePan = true;
        controls.enableZoom = true;

        return renderer.domElement;
    }

    function draw() {
        cube!.rotation.x += 0.01;
        cube!.rotation.y += 0.01;
        renderer!.render(scene!, camera!);
    }

    function resize(size: RectSize) {
        camera!.aspect = size.width / size.height;
        camera!.updateProjectionMatrix();
        renderer!.setSize(size.width, size.height);
        console.log("Resized to", size);
    }

    console.log("ThreeEmbed example loaded");

    import * as flatbuffers from 'flatbuffers';
    import {Data} from '$lib/flatbuffers/flatbuffers_generated';

    function loadFlatBuffer(buffer: Uint8Array) {
        const byteBuffer = new flatbuffers.ByteBuffer(buffer);
        const data = Data.getRootAsData(byteBuffer); // Deserialize

        console.log("Description:", data.description());
        console.log("Covers length:", data.coversLength());

        for (let i = 0; i < data.coversLength(); i++) {
            const cover = data.covers(i);
            console.log(`Cover ${i}: full = ${cover?.full()}`);
        }
        console.log("Loaded FlatBuffer:", data);
    }

    import * as fs from "fs";

    let buffer;

    async function loadData() {
        const response = await fetch('/test.bin'); // No backend needed!
        if (!response.ok) {
            console.error("Failed to load binary file");
            return;
        }
        buffer = new Uint8Array(await response.arrayBuffer());
        console.log("Loaded FlatBuffer:", buffer);
        loadFlatBuffer(buffer);
    }

    loadData();


    import {PaneGroup, Pane, PaneResizer} from "paneforge";
</script>


<PaneGroup class="h-full w-full">
    <Pane defaultSize={50}>
        <div style="padding: 1px; background-color: #00ff00; height: 100%; box-sizing: border-box;">
            <ThreeEmbed
                    onInit={setup}
                    onResize={resize}
                    onDestroy={() => renderer?.dispose()}
                    onTick={draw}/>
        </div>
    </Pane>
    <PaneResizer>
        <div style="width: 5px; background-color: #666666; height: 100%; box-sizing: border-box; cursor: col-resize;"></div>
    </PaneResizer>
    <Pane defaultSize={50}>
        <div style="padding: 1px; background-color: #00ff00; height: 100%; box-sizing: border-box;">
            <div id="duck-image" style="height: 100%"></div>
        </div>
    </Pane>
</PaneGroup>


<style>
    #duck-image {
        background-image: url(./duck.webp);
        background-size: cover;
        background-position: center
    }
</style>