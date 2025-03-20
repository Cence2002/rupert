<script lang="ts">
    import {onMount, onDestroy} from "svelte";
    import {watchResize} from "svelte-watch-resize";
    import {Vector2} from "three";
    import {lerp} from "three/src/math/MathUtils.js";

    export let setup: (width: number, height: number) => HTMLCanvasElement;
    export let draw: () => void;
    export let resize: (width: number, height: number) => void;
    export let onDoubleClick: (mouse: Vector2) => void = () => {
    };

    let container: HTMLDivElement;
    let canvas: HTMLCanvasElement;
    let animationFrame: number;
    let running = true;

    function handleDoubleClick(event: MouseEvent) {
        const mouse = new Vector2(
            lerp(-1, 1, event.offsetX / container.offsetWidth),
            lerp(1, -1, event.offsetY / container.offsetHeight)
        );
        onDoubleClick(mouse);
    }

    onMount(() => {
        canvas = setup(container.clientWidth, container.clientHeight);
        container.appendChild(canvas);

        canvas.addEventListener("dblclick", handleDoubleClick);

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
