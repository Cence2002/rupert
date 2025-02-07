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

    //print width and height of the window
    console.log(window.innerWidth, window.innerHeight);
</script>


<PaneGroup class="h-full w-full" direction="horizontal">
    <Pane defaultSize={2}>
        <PaneGroup class="h-full w-full" direction="vertical">
            <Pane defaultSize={1}>
                <div id="pane">
                    <div id="duck-image"></div>
                </div>
            </Pane>

            <PaneResizer>
                <div id="vertical-resizer" style="height: 10px; width: 100%;"></div>
            </PaneResizer>

            <Pane defaultSize={1}>
                <div id="pane">
                    <div id="duck-image"></div>
                </div>
            </Pane>
        </PaneGroup>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 10px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <ThreeEmbed
                    onInit={setup}
                    onResize={resize}
                    onDestroy={() => renderer?.dispose()}
                    onTick={draw}/>
        </div>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 10px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <div id="duck-image"></div>
        </div>
    </Pane>
</PaneGroup>


<style>
    #pane {
        padding: 2px;
        background-color: #00ff00;
        height: 100%;
        box-sizing: border-box;
    }

    #horizontal-resizer {
        background-color: #666666;
        cursor: col-resize;
        box-sizing: border-box;
    }

    #vertical-resizer {
        background-color: #666666;
        cursor: row-resize;
        box-sizing: border-box;
    }

    #duck-image {
        background-image: url(./duck.webp);
        background-size: cover;
        background-position: center;
        height: 100%;
    }
</style>