import os
import json
from openai import OpenAI

openai_api_key = os.getenv("OPENAI_API_KEY")

client = OpenAI(api_key=openai_api_key)

model = "o1"

helper_functions = """

static std::vector<Vertex<Interval>> permutations(const std::vector<Vertex<Interval>>& vertices) {
    std::vector<Vertex<Interval>> permutations;
    for(const Vertex<Interval>& vertex: vertices) {
        permutations.emplace_back(vertex.x(), vertex.y(), vertex.z());
        permutations.emplace_back(vertex.z(), vertex.x(), vertex.y());
        permutations.emplace_back(vertex.y(), vertex.z(), vertex.x());
        permutations.emplace_back(vertex.z(), vertex.y(), vertex.x());
        permutations.emplace_back(vertex.x(), vertex.z(), vertex.y());
        permutations.emplace_back(vertex.y(), vertex.x(), vertex.z());
    }
    return permutations;
}

static std::vector<Vertex<Interval>> rotations(const std::vector<Vertex<Interval>>& vertices) {
    std::vector<Vertex<Interval>> rotations;
    for(const Vertex<Interval>& vertex: vertices) {
        rotations.emplace_back(vertex.x(), vertex.y(), vertex.z());
        rotations.emplace_back(vertex.z(), vertex.x(), vertex.y());
        rotations.emplace_back(vertex.y(), vertex.z(), vertex.x());
    }
    return rotations;
}

static std::vector<Vertex<Interval>> flips(const Vertex<Interval>& vertex, const bool flip_x, const bool flip_y, const bool flip_z) {
    std::vector<Vertex<Interval>> flips;
    for(const bool sign_x: flip_x ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
        for(const bool sign_y: flip_y ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
            for(const bool sign_z: flip_z ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
                flips.emplace_back(
                    sign_x ? -vertex.x() : vertex.x(),
                    sign_y ? -vertex.y() : vertex.y(),
                    sign_z ? -vertex.z() : vertex.z()
                );
            }
        }
    }
    return flips;
}

static std::vector<Vertex<Interval>> flips(const Vertex<Interval>& vertex) {
    return flips({vertex}, vertex.x().is_nonzero(), vertex.y().is_nonzero(), vertex.z().is_nonzero());
}

static std::vector<Vertex<Interval>> combined(const std::vector<std::vector<Vertex<Interval>>>& vertices) {
    std::vector<Vertex<Interval>> combined;
    for(const std::vector<Vertex<Interval>>& vertex: vertices) {
        combined.insert(combined.end(), vertex.begin(), vertex.end());
    }
    return combined;
}

"""

example_output = """

JSON data for icosahedron example:

{
"name": "Icosahedron",
"constants": {
  "C0": "(1 + sqrt(5)) / 4"
},
"vertices": [
  [
    "0.5",
    "0.0",
    "C0"
  ],
  [
    "0.5",
    "0.0",
    "-C0"
  ],
  [
    "-0.5",
    "0.0",
    "C0"
  ],
  [
    "-0.5",
    "0.0",
    "-C0"
  ],
  [
    "C0",
    "0.5",
    "0.0"
  ],
  [
    "C0",
    "-0.5",
    "0.0"
  ],
  [
    "-C0",
    "0.5",
    "0.0"
  ],
  [
    "-C0",
    "-0.5",
    "0.0"
  ],
  [
    "0.0",
    "C0",
    "0.5"
  ],
  [
    "0.0",
    "C0",
    "-0.5"
  ],
  [
    "0.0",
    "-C0",
    "0.5"
  ],
  [
    "0.0",
    "-C0",
    "-0.5"
  ]
]
}

Output for icosahedron example:

template<IntervalType I>
static Polyhedron<I> icosahedron() {
    const I c0 = (I(1) + I(5).sqrt()) / 4;
    return Polyhedron<I>(Polyhedron<I>::rotations(Polyhedron<I>::flips(Vertex<I>(I(1) / 2, I(0), c0))));
}


JSON data for dodecahedron example:

{
"name": "Dodecahedron",
"constants": {
  "C0": "(1 + sqrt(5)) / 4",
  "C1": "(3 + sqrt(5)) / 4"
},
"vertices": [
  [
    "0.0",
    "0.5",
    "C1"
  ],
  [
    "0.0",
    "0.5",
    "-C1"
  ],
  [
    "0.0",
    "-0.5",
    "C1"
  ],
  [
    "0.0",
    "-0.5",
    "-C1"
  ],
  [
    "C1",
    "0.0",
    "0.5"
  ],
  [
    "C1",
    "0.0",
    "-0.5"
  ],
  [
    "-C1",
    "0.0",
    "0.5"
  ],
  [
    "-C1",
    "0.0",
    "-0.5"
  ],
  [
    "0.5",
    "C1",
    "0.0"
  ],
  [
    "0.5",
    "-C1",
    "0.0"
  ],
  [
    "-0.5",
    "C1",
    "0.0"
  ],
  [
    "-0.5",
    "-C1",
    "0.0"
  ],
  [
    "C0",
    "C0",
    "C0"
  ],
  [
    "C0",
    "C0",
    "-C0"
  ],
  [
    "C0",
    "-C0",
    "C0"
  ],
  [
    "C0",
    "-C0",
    "-C0"
  ],
  [
    "-C0",
    "C0",
    "C0"
  ],
  [
    "-C0",
    "C0",
    "-C0"
  ],
  [
    "-C0",
    "-C0",
    "C0"
  ],
  [
    "-C0",
    "-C0",
    "-C0"
  ]
]
}

Output for dodecahedron example:

template<IntervalType I>
static Polyhedron<I> dodecahedron() {
    const I c0 = (1 + I(5).sqrt()) / 4;
    const I c1 = (3 + I(5).sqrt()) / 4;
    return Polyhedron(Polyhedron<I>::combined({
        Polyhedron<I>::rotations(Polyhedron<I>::flips(Vertex<I>(I(0), I(1) / 2, c1))),
        Polyhedron<I>::flips(Vertex<I>(c0, c0, c0))
    }));
}

"""

base_prompt = f"""
You are an expert C++ code generator. Your task is to generate a C++ function for a polyhedron based on its description.
You have access to these helper functions:

{helper_functions}

When generating the code, analyze the list of vertices to uncover any inherent patterns.
Look for groups of vertices where the coordinates follow a similar structure and can be represented by a single instance.
For example, if multiple vertices can be derived by applying rotations(), permutations(), or even flips() on a representative vertex, use that structure to simplify the definition of the polyhedron.
The polyhedron description is provided as a name, a set of constants, and a list of vertices.

Here are a few examples of what I would like you to generate. For each example, I first show you the JSON data, and an example for a nice output:

{example_output}

Always use snake case for the function name, and lowercase variable names, with consistent naming as in the example.
All coordinates need to be intervals, even simple integers. Use the I(int) method to generate these. Note that Intervals can only be initialized with integer constants,
so you must define fractions like 0.5 as I(1) / 2
Exclude all comments and output only the generated C++ (templated) function code (raw text output), without any additional information or context (even the code block ```cpp).

The JSON data is provided below, please remember all tasks, and think through the answer carefully before generating the single function for this polyhedron!

"""

all_generated_code = ""

with open("../scrape/polyhedra.json", "r") as json_file:
    polyhedra = json.load(json_file)

for category, polyhedron_list in polyhedra["Polyhedra"].items():
    for polyhedron in polyhedron_list:
        if polyhedron["name"] != "Rhombicosidodecahedron":
            continue

        prompt = (
            f"{base_prompt}\n\n"
            f"Polyhedron Name: {polyhedron['name']}\n"
            f"Constants: {json.dumps(polyhedron['constants'])}\n"
            f"Vertices: {json.dumps(polyhedron['vertices'])}\n\n"
            "Generate the corresponding C++ function."
        )

        response = client.responses.create(
            model=model,
            instructions="You are an expert C++ code generator that converts structured symbolic JSON polyhedron definition data into functions using helper functions.",
            input=prompt,
        )

        print(response.output_text)
        exit(0)

        all_generated_code += generated_function + "\n\n"

with open("generated_polyhedra.cpp", "w") as output_file:
    output_file.write(all_generated_code)

print("C++ code for the polyhedra functions has been generated and saved to 'generated_polyhedra.cpp'.")
