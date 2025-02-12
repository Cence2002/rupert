<script lang="ts">
    import {onMount, onDestroy} from "svelte";
    import {watchResize} from "svelte-watch-resize";

    export let setup: (width: number, height: number) => HTMLCanvasElement;
    export let draw: () => void;
    export let resize: (width: number, height: number) => void;

    let container: HTMLDivElement;
    let canvas: HTMLCanvasElement;
    let animationFrame: number;
    let running = true;

    onMount(() => {
        canvas = setup(container.clientWidth, container.clientHeight);
        container.appendChild(canvas);

        function animate() {
            if (!running) {
                return;
            }
            animationFrame = requestAnimationFrame(animate);
            draw();
        }

        animate();
    });

    function updateContainerSize() {
        resize(container.clientWidth, container.clientHeight);
    }

    onDestroy(() => {
        running = false;
        cancelAnimationFrame(animationFrame);
        if (container && canvas && container.contains(canvas)) {
            container.removeChild(canvas);
        }
        location.reload();
    });
</script>

<div style="width: 100%; height: 100%; display: flex; justify-content: center; align-items: center;"
     bind:this={container}
     use:watchResize={updateContainerSize}>
</div>
