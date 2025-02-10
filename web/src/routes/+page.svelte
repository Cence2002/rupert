<script lang="ts">
    import {PaneGroup, Pane, PaneResizer} from "paneforge";

    import Parameter3d from "$lib/Parameter3d.svelte";
    import Parameter2d from "$lib/Parameter2d.svelte";

    import {loadData} from "$lib/loadData";

    loadData("/test.bin").then(data => console.log(data.description()));
</script>


<PaneGroup class="h-full w-full" direction="horizontal">
    <Pane defaultSize={2}>
        <PaneGroup class="h-full w-full" direction="vertical">
            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter3d/>
                </div>
            </Pane>

            <PaneResizer>
                <div id="vertical-resizer" style="height: 5px; width: 100%;"></div>
            </PaneResizer>

            <Pane defaultSize={1}>
                <div id="pane">
                    <Parameter2d/>
                </div>
            </Pane>
        </PaneGroup>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 5px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <Parameter3d/>
        </div>
    </Pane>

    <PaneResizer>
        <div id="horizontal-resizer" style="width: 5px; height: 100%;"></div>
    </PaneResizer>

    <Pane defaultSize={3}>
        <div id="pane">
            <Parameter2d/>
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