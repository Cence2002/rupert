<script lang="ts">
    import {PaneGroup, Pane, PaneResizer} from "paneforge";

    import Parameter3 from "$lib/Parameter3.svelte";
    import Parameter2 from "$lib/Parameter2.svelte";
    import Geometry3 from "$lib/Geometry3.svelte";
    import Geometry2 from "$lib/Geometry2.svelte";


    import {onMount} from 'svelte';
    import {loadData} from "$lib/loader";

    onMount(async () => {
        let data = await loadData("/test.bin");
        console.log(data);
    });
</script>


<PaneGroup class="h-full w-full" direction="horizontal">
    <Pane defaultSize={2}>
        <PaneGroup class="h-full w-full" direction="vertical">
            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter3/>
                </div>
            </Pane>

            <PaneResizer>
                <div id="vertical-resizer" style="height: 5px; width: 100%;"></div>
            </PaneResizer>

            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter2/>
                </div>
            </Pane>
        </PaneGroup>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 5px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <Geometry3/>
        </div>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 5px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <Geometry2/>
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

    #duck-image {
        background-image: url(./duck.webp);
        background-size: cover;
        background-position: center;
        height: 100%;
    }
</style>