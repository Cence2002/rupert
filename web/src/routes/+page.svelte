<script lang="ts">
    import {PaneGroup, Pane, PaneResizer} from "paneforge";
    import {Cover} from '$lib/flatbuffers/flatbuffers_generated';
    import {Selection} from "$lib/state.svelte";

    import Parameter3 from "$lib/Parameter3.svelte";
    import Parameter2 from "$lib/Parameter2.svelte";
    import Geometry3 from "$lib/Geometry3.svelte";
    import Geometry2 from "$lib/Geometry2.svelte";

    import {onMount} from 'svelte';
    import {loadCover} from "$lib/loader";

    let cover: Cover | undefined = $state();
    let selection = new Selection();

    onMount(async () => {
        cover = await loadCover("/test.bin");
        console.log("Fetched:", cover!.description());
    });
</script>


<!--<button onclick={() => selection.selectBox3(selection.selectedBox3 === null ? 1 : selection.selectedBox3 + 1)}>Increment Box3</button>-->
<!--<button onclick={() => selection.selectBox2(selection.selectedBox2 === null ? 1 : selection.selectedBox2 + 1)}>Increment Box2</button>-->
<!--<p>Selected Box3 Index: {selection.selectedBox3 === null ? "null" : selection.selectedBox3}</p>-->
<!--<p>Selected Box2 Index: {selection.selectedBox2 === null ? "null" : selection.selectedBox2}</p>-->

<PaneGroup class="h-full w-full" direction="horizontal">
    <Pane defaultSize={2}>
        <PaneGroup class="h-full w-full" direction="vertical">
            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter3 {cover} {selection}/>
                </div>
            </Pane>

            <PaneResizer>
                <div id="vertical-resizer" style="height: 5px; width: 100%;"></div>
            </PaneResizer>

            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter2 {cover} {selection}/>
                </div>
            </Pane>
        </PaneGroup>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 4px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <Geometry3 {cover} {selection}/>
        </div>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 4px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <Geometry2 {cover} {selection}/>
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
