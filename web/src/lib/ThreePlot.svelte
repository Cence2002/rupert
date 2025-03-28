<script lang="ts">
    import {onMount, onDestroy} from "svelte";
    import {watchResize} from "svelte-watch-resize";
    import {Vector2} from "three";
    import {lerp} from "three/src/math/MathUtils.js";

    export let setup: (width: number, height: number) => HTMLCanvasElement;
    export let draw: () => void;
    export let resize: (width: number, height: number) => void;
    export let onClick: (mouse: Vector2) => void = () => {
    };

    let container: HTMLDivElement;
    let canvas: HTMLCanvasElement;
    let animationFrame: number;
    let running = true;
    let mouseDown: Vector2 | null = null;

    const mouseMoveThreshold = 0.01;

    function getMouse(event: MouseEvent): Vector2 {
        return new Vector2(
            lerp(-1, 1, event.offsetX / container.offsetWidth),
            lerp(1, -1, event.offsetY / container.offsetHeight),
        );
    }

    function onMouseDown(event: MouseEvent) {
        mouseDown = getMouse(event);
    }

    function onMouseUp(event: MouseEvent) {
        const mouseUp = getMouse(event);
        if (mouseDown && mouseDown.distanceTo(mouseUp) < mouseMoveThreshold) {
            onClick(mouseUp);
        }
        mouseDown = null;
    }

    onMount(() => {
        canvas = setup(container.clientWidth, container.clientHeight);
        container.appendChild(canvas);

        canvas.addEventListener("mousedown", onMouseDown);
        canvas.addEventListener("mouseup", onMouseUp);

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
