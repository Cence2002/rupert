<script lang="ts">
    import {Cover} from '$lib/flatbuffers/flatbuffers_generated';

    import {onMount} from 'svelte';
    import {loadCover} from "$lib/loader";

    let cover: Cover | undefined = $state();

    onMount(async () => {
        cover = await loadCover("/test.bin");
        console.log("fetched", cover.description());
    });

    let selectedBox3: number | null = $state(null);
    let selectedBox2: number | null = $state(null);

    function selectBox3(index: number) {
        selectedBox2 = null;
        selectedBox3 = index;
    }

    function selectBox2(index: number) {
        if (selectedBox3 === null) {
            return;
        }
        selectedBox2 = index;
    }

    import {PaneGroup, Pane, PaneResizer} from "paneforge";

    import Parameter3 from "$lib/Parameter3.svelte";
    import Parameter2 from "$lib/Parameter2.svelte";
    import Geometry3 from "$lib/Geometry3.svelte";
    import Geometry2 from "$lib/Geometry2.svelte";
</script>


<button onclick={() => selectBox3(selectedBox3 === null ? 1 : selectedBox3 + 1)}>Increment Box3</button>
<button onclick={() => selectBox2(selectedBox2 === null ? 1 : selectedBox2 + 1)}>Increment Box2</button>
<p>Selected Box3 Index: {selectedBox3 === null ? "null" : selectedBox3}</p>
<p>Selected Box2 Index: {selectedBox2 === null ? "null" : selectedBox2}</p>

<PaneGroup class="h-full w-full" direction="horizontal">
    <Pane defaultSize={2}>
        <PaneGroup class="h-full w-full" direction="vertical">
            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter3 bind:cover bind:selectedBox3 {selectBox3}/>
                </div>
            </Pane>

            <PaneResizer>
                <div id="vertical-resizer" style="height: 5px; width: 100%;"></div>
            </PaneResizer>

            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter2 bind:cover bind:selectedBox3 bind:selectedBox2 {selectBox2}/>
                </div>
            </Pane>
        </PaneGroup>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 5px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <Geometry3 bind:cover bind:selectedBox3 bind:selectedBox2/>
        </div>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 5px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <Geometry2 bind:cover bind:selectedBox3 bind:selectedBox2/>
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
        background-color: #666666;
        cursor: col-resize;
        box-sizing: border-box;
    }

    #vertical-resizer {
        background-color: #666666;
        cursor: row-resize;
        box-sizing: border-box;
    }
</style>
