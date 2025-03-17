<script lang="ts">
    import {PaneGroup, Pane, PaneResizer} from "paneforge";
    import {Boxes} from '$lib/flatbuffers/flatbuffers_generated';
    import {Selection} from "$lib/state.svelte";

    import Parameter3 from "$lib/plots/Boxes.svelte";
    import Parameter2 from "$lib/plots/Rectangles.svelte";
    import Geometry3 from "$lib/plots/Main.svelte";
    import Geometry2 from "$lib/plots/Projection.svelte";

    import {onMount} from 'svelte';
    import {loadBoxes} from "$lib/loader";

    let boxes: Boxes | undefined = $state();
    let selection = new Selection();

    let description = "RID";
    let terminalBoxesFilename = description + "_terminal_boxes.bin";
    let debugFilename = description + "_debug.bin";

    onMount(async () => {
        boxes = await loadBoxes(debugFilename);
        console.log("Loaded:", boxes!.description());
    });
</script>


<PaneGroup class="h-full w-full" direction="horizontal">
    <Pane defaultSize={2}>
        <PaneGroup class="h-full w-full" direction="vertical">
            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter3 {boxes} {selection}/>
                </div>
            </Pane>

            <PaneResizer>
                <div id="vertical-resizer" style="height: 5px; width: 100%;"></div>
            </PaneResizer>

            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter2 {boxes} {selection}/>
                </div>
            </Pane>
        </PaneGroup>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 4px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={4}>
        <div id="pane">
            <Geometry3 {boxes} {selection}/>
        </div>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 4px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={2}>
        <div id="pane">
            <Geometry2 {boxes} {selection}/>
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
