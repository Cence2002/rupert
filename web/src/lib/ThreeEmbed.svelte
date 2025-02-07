<script lang="ts">
    // Inputs of the component:
    import {onMount} from "svelte";
    import {assert, type RectSize} from "$lib";
    import {watchResize} from "svelte-watch-resize";

    interface Props {
        onInit: (containerSize: RectSize) => HTMLCanvasElement; // must return the canvas element to insert
        onTick: () => void;
        onDestroy: () => void;
        onResize: (containerSize: RectSize) => void;
    }

    let {onInit, onTick, onDestroy, onResize}: Props = $props();

    let containerElement: HTMLDivElement | null = $state(null);

    onMount(() => {
        assert(containerElement);

        let containerSize: RectSize = {
            width: containerElement.clientWidth,
            height: containerElement.clientHeight
        };

        const canvas = onInit(containerSize);
        containerElement.appendChild(canvas);

        function animate() {
            requestAnimationFrame(animate);
            onTick();
        }

        animate();

        return () => {
            onDestroy();

            assert(containerElement);
            containerElement.removeChild(canvas);
        }
    });

    function updateContainerSize() {
        assert(containerElement);
        onResize({width: containerElement.clientWidth, height: containerElement.clientHeight});
    }

</script>
<!-- detect if in frontend mode -->
<div style="width: 100%; height: 100%; overflow: hidden"
     bind:this={containerElement}
     use:watchResize={updateContainerSize}>
</div>
