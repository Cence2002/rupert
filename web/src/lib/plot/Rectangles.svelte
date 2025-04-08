<script lang="ts">
    import ThreePlot from "$lib/ThreePlot.svelte";
    import {State} from "$lib/State.svelte";

    import {MapControls} from 'three/addons/controls/MapControls.js';
    import {
        PlaneGeometry,
        MeshBasicMaterial,
        OrthographicCamera,
        Scene,
        WebGLRenderer,
        EdgesGeometry,
        LineBasicMaterial,
        LineSegments,
        AxesHelper,
        Vector2,
        Raycaster,
        FrontSide,
        Color,
        BufferGeometry,
        Float32BufferAttribute,
        Matrix4,
        Vector3,
        InstancedMesh,
        Quaternion,
        Mesh, CircleGeometry,
    } from "three";
    import type {AbstractLoader} from "$lib/loader/AbstractLoader";
    import {angleBetweenMatrices, PI, projectionMatrix, symmetries, transformationMatrix} from "$lib/Geometry";

    const {loader, state} = $props<{
        loader: AbstractLoader,
        state: State,
    }>();

    $effect(onSelectBox);

    $effect(onSelectRectangle);

    const scene = new Scene();
    scene.up.set(0, 1, 0);

    const camera = new OrthographicCamera(0, 0, 0, 0, -1, 1);
    camera.up.set(0, 1, 0);
    camera.lookAt(0.5, 0.5, 0);
    camera.position.set(0, 0, 0);

    const renderer = new WebGLRenderer({antialias: true});

    const controls = new MapControls(camera, renderer.domElement);
    controls.enablePan = true;
    controls.screenSpacePanning = true;
    controls.enableRotate = false;
    controls.enableZoom = true;
    controls.zoomToCursor = true;
    controls.minZoom = 0.01;
    controls.maxZoom = 100;

    let rectangles: InstancedMesh | null = null;
    let rectangleEdges: LineSegments | null = null;

    const rectangleMaterial = new MeshBasicMaterial({
        color: new Color(0, 1, 0),
        transparent: true,
        opacity: 0.25,
        side: FrontSide,
        depthWrite: false
    });

    const rectangleInMaterial = new MeshBasicMaterial({
        color: new Color(0, 1, 0),
        transparent: false,
        side: FrontSide,
    });

    const rectangleEdgesMaterial = new LineBasicMaterial({
        color: new Color(0.5, 1, 0.5),
        transparent: false
    });

    const selectedRectangleEdgeMaterial = new LineBasicMaterial({
        color: new Color(1, 0, 0),
        transparent: true
    });

    function onSelectBox(): () => void {
        if (state.selectedBox === null) {
            return () => {
            };
        }

        const rectangleGeometry = new PlaneGeometry(1, 1);
        rectangles = new InstancedMesh(rectangleGeometry, rectangleMaterial, loader.getRectangleCount(state.selectedBox));

        const vertices = [
            new Vector3(-0.5, -0.5, 0),
            new Vector3(0.5, -0.5, 0),
            new Vector3(0.5, 0.5, 0),
            new Vector3(-0.5, 0.5, 0)
        ];
        const vertexIndices = [
            [0, 1], [1, 2], [2, 3], [3, 0]
        ];

        const matrix = new Matrix4();
        const edgeBuffer: number[] = [];
        for (const [rectangleIndex, rectangle] of loader.getRectangles(state.selectedBox).entries()) {
            matrix.identity();
            matrix.makeTranslation(rectangle.position());
            matrix.scale(rectangle.scale());
            rectangles.setMatrixAt(rectangleIndex, matrix);

            if (!rectangle.terminal) {
                continue;
            }
            const transformedVertices = vertices.map(vertex => vertex.clone().applyMatrix4(matrix));
            for (const [vertexIndex, otherVertexIndex] of vertexIndices) {
                const vertex = transformedVertices[vertexIndex];
                const otherVertex = transformedVertices[otherVertexIndex];
                edgeBuffer.push(vertex.x, vertex.y, vertex.z);
                edgeBuffer.push(otherVertex.x, otherVertex.y, otherVertex.z);
            }
        }
        scene.add(rectangles);

        const rectangleEdgesGeometry = new BufferGeometry();
        rectangleEdgesGeometry.setAttribute('position', new Float32BufferAttribute(edgeBuffer, 3));
        rectangleEdges = new LineSegments(rectangleEdgesGeometry, rectangleEdgesMaterial);
        scene.add(rectangleEdges);

        let inRectangle: Mesh | null = null;
        {
            const inRectangleIndex: number | null = loader.getInRectangleIndex(state.selectedBox);
            if (inRectangleIndex !== null) {
                const inRectangleTemp = loader.getRectangle(state.selectedBox, inRectangleIndex)!;
                const inRectangleGeometry = new PlaneGeometry(1, 1);
                inRectangle = new Mesh(inRectangleGeometry, rectangleInMaterial);
                inRectangle.position.copy(inRectangleTemp.position());
                inRectangle.scale.copy(inRectangleTemp.scale());
                scene.add(inRectangle);
            }
        }

        return () => {
            scene.remove(rectangles!);
            rectangles = null;

            scene.remove(rectangleEdges!);
            rectangleEdges = null;

            if (inRectangle !== null) {
                scene.remove(inRectangle);
                inRectangle = null;
            }
        };
    }

    function onSelectRectangle() {
        if (state.selectedRectangle === null || !rectangles) return;

        const rectangle = loader.getRectangle(state.selectedBox, state.selectedRectangle);

        const selectedRectangleGeometry = new PlaneGeometry(1, 1);
        const selectedRectangleEdgesGeometry = new EdgesGeometry(selectedRectangleGeometry);
        const selectedRectangle = new LineSegments(selectedRectangleEdgesGeometry, selectedRectangleEdgeMaterial);
        selectedRectangle.position.copy(rectangle.position());
        selectedRectangle.scale.copy(rectangle.scale());

        scene.add(selectedRectangle);

        const epsilon = PI / 180 * 10;

        const box = loader.getBox(state.selectedBox)
        const boxRadius = new Vector2(box.theta.interval.len / 2 + box.alpha.interval.len / 2, box.phi.interval.len / 2).length();
        const holeMatrix = transformationMatrix(box.theta.interval.mid, box.phi.interval.mid, box.alpha.interval.mid);
        const plugMatrix = projectionMatrix(rectangle.theta.interval.mid, rectangle.phi.interval.mid);
        let angleBetween = 1e6;
        const {rotations, reflections} = symmetries(loader.getHole(), 1e-6);
        let angles = [];
        for (const rotation of rotations) {
            const rotationMatrix = new Matrix4();
            rotationMatrix.set(
                rotation.elements[0], rotation.elements[1], rotation.elements[2], 0,
                rotation.elements[3], rotation.elements[4], rotation.elements[5], 0,
                rotation.elements[6], rotation.elements[7], rotation.elements[8], 0,
                0, 0, 0, 1
            );
            angles.push(Math.round(angleBetweenMatrices(rotationMatrix, new Matrix4().identity()) * 180 / PI));

            angleBetween = Math.min(angleBetween, angleBetweenMatrices(holeMatrix.clone().multiply(rotationMatrix), plugMatrix));
        }
        //print sorted angles
        angles.sort((a, b) => a - b);
        //print the counts of each element as a map
        const counts = new Map<number, number>();
        for (const angle of angles) {
            if (counts.has(angle)) {
                counts.set(angle, counts.get(angle)! + 1);
            } else {
                counts.set(angle, 1);
            }
        }
        console.log('counts', counts);
        const ZreflectionMatrix = new Matrix4();
        ZreflectionMatrix.set(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, -1, 0,
            0, 0, 0, 1
        );
        for (const reflection of reflections) {
            const reflectionMatrix = new Matrix4();
            reflectionMatrix.set(
                reflection.elements[0], reflection.elements[1], reflection.elements[2], 0,
                reflection.elements[3], reflection.elements[4], reflection.elements[5], 0,
                reflection.elements[6], reflection.elements[7], reflection.elements[8], 0,
                0, 0, 0, 1
            );
            angleBetween = Math.min(angleBetween, angleBetweenMatrices(holeMatrix.clone().multiply(reflectionMatrix), ZreflectionMatrix.clone().multiply(plugMatrix)));
        }
        const remainingAngle = epsilon - boxRadius - angleBetween;
        //print in degrees
        console.log('remainingAngle', remainingAngle * 180 / Math.PI);
        // if (remainingAngle > 0) {
        //create a circle with radius = remainingAngle, center = rectangle.position
        const circleGeometry = new CircleGeometry(remainingAngle, 32);
        const circleMaterial = new MeshBasicMaterial({
            color: new Color(1, 0, 0),
            transparent: true,
            opacity: 0.5,
            side: FrontSide,
            depthWrite: false,
            wireframe: true,
        });
        const circle = new Mesh(circleGeometry, circleMaterial);
        circle.position.copy(rectangle.position());
        circle.scale.multiplyScalar(Math.max(0, remainingAngle));
        scene.add(circle);

        return () => {
            scene.remove(selectedRectangle);
            scene.remove(circle);
        };
    }

    function onClick(mouse: Vector2) {
        if (rectangles === null) {
            return;
        }

        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        const intersectedInstances = raycaster.intersectObject(rectangles, false);

        if (intersectedInstances.length === 0) {
            state.unselectRectangle();
            return;
        }

        function getArea(matrix: Matrix4) {
            const scale = new Vector3();
            matrix.decompose(new Vector3(), new Quaternion(), scale);
            return scale.x * scale.y;
        }

        intersectedInstances.sort((intersectedInstance, otherIntersectedInstance) => {
            const intersectedInstanceMatrix = new Matrix4();
            const otherIntersectedInstanceMatrix = new Matrix4();
            rectangles!.getMatrixAt(intersectedInstance.instanceId!, intersectedInstanceMatrix);
            rectangles!.getMatrixAt(otherIntersectedInstance.instanceId!, otherIntersectedInstanceMatrix);
            const area = getArea(intersectedInstanceMatrix);
            const otherArea = getArea(otherIntersectedInstanceMatrix);
            const distanceDifference = intersectedInstance.distance - otherIntersectedInstance.distance;
            return distanceDifference - Math.log(area / otherArea) / 1000;
        });

        const instanceIds = intersectedInstances.map(i => i.instanceId);

        if (state.selectedRectangle === null || !instanceIds.includes(state.selectedRectangle)) {
            state.selectRectangle(instanceIds[0]);
            return;
        }

        const selectedInstanceIdIndex = instanceIds.indexOf(state.selectedRectangle);
        const newSelectedInstanceIdIndex = (selectedInstanceIdIndex + 1) % instanceIds.length;
        state.selectRectangle(instanceIds[newSelectedInstanceIdIndex]);
    }

    function setup(width: number, height: number) {
        resize(width, height);

        const axesHelper = new AxesHelper(2);
        scene.add(axesHelper);

        const edgeBuffer = [
            1, 1, 0, 1, 0, 0,
            1, 1, 0, 0, 1, 0,
        ];
        const domainEdgesGeometry = new BufferGeometry();
        domainEdgesGeometry.setAttribute('position', new Float32BufferAttribute(edgeBuffer, 3));
        const domainEdgesMaterial = new LineBasicMaterial({
            color: new Color(1, 1, 1),
            transparent: true,
            opacity: 0.5,
        });
        const domainEdges = new LineSegments(domainEdgesGeometry, domainEdgesMaterial);
        scene.add(domainEdges);

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number, zoom: number = 1.5) {
        const aspect = width / height;
        camera.left = (1 - zoom * aspect) / 2;
        camera.right = (1 + zoom * aspect) / 2;
        camera.top = (1 + zoom) / 2;
        camera.bottom = (1 - zoom) / 2;
        camera.updateProjectionMatrix();

        renderer.setSize(width, height);

        controls.update();
    }
</script>

<ThreePlot {setup} {resize} {draw} {onClick}/>
