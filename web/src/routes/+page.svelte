<script lang="ts">
    import {PaneGroup, Pane, PaneResizer} from "paneforge";
    import {State} from "$lib/state.svelte";

    import Boxes from "$lib/plot/Boxes.svelte";
    import Rectangles from "$lib/plot/Rectangles.svelte";
    import Main from "$lib/plot/Main.svelte";
    import Projection from "$lib/plot/Projection.svelte";

    import {onMount} from 'svelte';
    import {DebugLoader} from "$lib/loader/debug";
    import type {Scene} from "three";

    let state = new State();

    let description = "RID";
    // let terminalBoxesFilename = description + "_terminal_boxes.bin";
    let debugFilename = description + "_debug.bin";

    let loader = new DebugLoader();

    let projectionScene: Scene | null = null;

    function setProjectionScene(scene: Scene) {
        projectionScene = scene;
    }

    function getProjectionScene() {
        return projectionScene;
    }

    onMount(async () => {
        await loader.load(debugFilename);
        state.onLoaded();
    });
</script>


<PaneGroup class="h-full w-full" direction="horizontal">
    <Pane defaultSize={2}>
        <PaneGroup class="h-full w-full" direction="vertical">
            <Pane defaultSize={1}>
                <div id="pane">
                    <Boxes {loader} {state}/>
                </div>
            </Pane>

            <PaneResizer>
                <div id="vertical-resizer" style="height: 5px; width: 100%;"></div>
            </PaneResizer>

            <Pane defaultSize={1}>
                <div id="pane">
                    <Rectangles {loader} {state}/>
                </div>
            </Pane>
        </PaneGroup>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 4px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={4}>
        <div id="pane">
            <Main {loader} {state} {getProjectionScene}/>
        </div>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 4px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={2}>
        <div id="pane">
            <Projection {loader} {state} {setProjectionScene}/>
        </div>
    </Pane>
</PaneGroup>


<style>
    #pane {
        background-color: #000000;
        height: 100%;
        box-sizing: border-box;
    }

    #horizontal-resizer {
        background-color: #444444;
        cursor: col-resize;
        box-sizing: border-box;
    }

    #vertical-resizer {
        background-color: #444444;
        cursor: row-resize;
        box-sizing: border-box;
    }
</style>
