# Prince Rupert's Cube

Outline of the algorithm for future me:

- only examine centrally symmetric polyhedra -> 5d search space (instead of 7d) because no translation is needed
- use a global solver to rule out cases where the total relative rotation bigger than some arbitrary epsilon:
    - represent each dimension of each box as an interval between two dyadic rational multiples of pi (as all are angles)
    - recurse in 3d space of angles for the hole, build a convex hull that definitely contains each projection of all vertices
    - for each 3d box:
        - prune it if the box is repeatedly evaluated due to symmetry
        - prune if the two objects are definitely within epsilon rotation of each other
        - recurse in 2d space of angles for the plug object
            - prune it if the box is repeatedly evaluated due to symmetry
            - the projection of the projected plug vertex is bounded by two lines and two arcs of ellipses
            - prune if this region is definitely outside the convex hull of the hole projection
            - if any such projection region is definitely inside the convex hull of the hole projection, we won't be able to prune this 3d box
        - bisecting each box in each dimension if it cannot be pruned (if we hit a minimum box size, we can give up)
    - when this terminates (if all remaining boxes are smaller than a threshold):
        - we collect each pair of 3d/2d boxes that we couldn't prune, group them by symmetry, and log them as "potential local regions" (these will be small)
- here, the only cases that remain are:
    - if the two objects' orientations are very close to each other (within epsilon rotation) (small neighborhood of a 2d manifold in 5d space)
    - their orientation combinations are very close to the potential local regions returned by the global solver (small neighborhood of a 0d manifold in 5d space)
- now we use the local solver to rule out the cases where the relative orientations are close, but there is:
    - no potentially vertical face (no face normal is within epsilon of parallel to the projection plane) (small neighborhood of a 1d manifold in 5d space)
    - no vertex potentially on the "apex" (no center-to-vertex vector is within epsilon of parallel to the projection plane) (small neighborhood of a 2d manifold in 5d space)
    - hopefully, there is an AND connection between these for the right polyhedra, so we reduce the potential region to a small neighborhood of a set of points (0d manifold)
    - For the rhombicosidodecahedron (RID), there is an AND connection (enumerated by hand)!!!
- We now reparametrize the 5d space, instead of using:
    - 3d for the first object
    - 2d for the second object
- we use:
    - 2d for the orientation of the first object
    - 2d for the axis of rotation between the two objects
    - 1d for the angle of rotation between the two objects (constantly any value below the global solver's epsilon)
- for the local solver, we recurse in the 2d space of the first object:
    - prune duplicate boxes due to symmetry
    - find all vertices that are definitely on the silhouette (easy to figure out by checking which side of the spherical great circles we are on (that are perpendicular to the face normals)):
        - a vertex is in the silhouette if it's part of two faces, one definitely front-facing and one definitely back-facing (one strictly positive and one strictly negative dot product)
    - we now recurse in the 2d space of the rotation axis:
        - if any of the silhouette vertex's projection constantly shifts outward of the silhouette projection of the hole, we can prune this box
        - we can check this simply by projecting Rodrigues' formula's derivative to 2d and checking (with interval arithmetic) if it has a positive cross product with the respective silhouette edge
        - a bit smarter way is to use an ellipse-line intersection again, but ignore the intersection when the rotation angle is 0 (here, it's important to handle the case when it's not guaranteed
          that the vertex starts to move out, this can happen especially for large boxes, but it's easy to handle)
- Now, for well-behaved polyhedra (like the RID), we ruled out most of the 5d space except for arbitrarily small neighborhoods of a few points, that can either be:
    - Cases where the two polyhedra are very close to each other (within epsilon rotation) -> for RID, there is a single such point, where a square face is parallel to the projection plane
    - Cases where the two polyhedra are close to another potential local region (both of their orientations defined) -> there is a single such case for RID (where a square face is vertical for both
      objects, and for the plug, another square is horizontal, and for the hole, two pentagon edge projection coincide with this horizontal square)
- for the first, we use a local rule by solving the following problem:
    - we first ask: what are all the axes around which we can rotate a vertex so that it's component in a given direction (perpendicular to the projection of an edge coming out of it) doesn't get
      longer
        - the unit axes is an intersection of a sphere and a parabolic hyperboloid (shifted in the z-axis, which is orthogonal both the vertex and the component direction)
    - solving for two edges in two opposite-ish directions gives us a banana shaped intersection of two such surfaces, which is arbitrarily narrow in the short direction if we choose small enough
      epsilon, and is shorter than pi in the longer direction
    - to have a local passage, we would need to find an orientation of the hole where ALL these bananas intersect (this is necessary, but still not sufficient, because it doesn't guarantee that all
      other vertices are within, and that the silhouette vertices are within edges that are not coming out of them)
    - (evaluated by a quick demo and by hand) for the RID, there is no such orientation in the small neighborhood (where the silhouette is any 3 of the 4 possible silhouettes that contain all cases in
      the neighborhood of a parallel square face)
- for the second, a special application of these banana-shapes shows that the square faces that are proved to be approximately vertical by the solvers would have to be precisely vertical (which means
  they can't pass, since their projections coincide)


### Progress
- big milestone: global solver is mostly done!!!
- the local solver is fully planned out, and many parts of the global solver can be reused, its results are known (as above) due to manual examination of RID
- the elementary geometric proof for the final cases is fully refined on paper TODO write it up in a formal paper

## Goal
- finalise and prettify the global solver (speed up if necessary)
- implement the local solver (only a few modifications needed compared to the global solver)
- write up the final geometric proof
- collect all pieces with nice visualisations and demos and publish this as:
#### Disproving the Generalized Rupert Conjecture
### Heavily using computer-assisted methods, and the RID is the first known counterexample
