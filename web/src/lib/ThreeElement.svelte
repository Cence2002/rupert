<script lang="ts">
    import {onMount, onDestroy} from "svelte";
    import {watchResize} from "svelte-watch-resize";

    export let setup: (width: number, height: number) => HTMLCanvasElement;
    export let draw: () => void;
    export let resize: (width: number, height: number) => void;

    let container: HTMLDivElement;
    let canvas: HTMLCanvasElement;

    onMount(() => {
        canvas = setup(container.clientWidth, container.clientHeight);
        container.appendChild(canvas);

        function animate() {
            requestAnimationFrame(animate);
            draw();
        }

        animate();
    });

    function updateContainerSize() {
        resize(container.clientWidth, container.clientHeight);
    }

    onDestroy(() => {
        container.removeChild(canvas);
    });
</script>

<div style="width: 100%; height: 100%; display: flex; justify-content: center; align-items: center;"
     bind:this={container}
     use:watchResize={updateContainerSize}>
</div>
