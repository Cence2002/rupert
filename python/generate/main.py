import os
import json
import asyncio

from openai import OpenAI, AsyncOpenAI
from dotenv import load_dotenv

dotenv_path = os.path.join(os.path.dirname(__file__), '.env')
load_dotenv(dotenv_path)

client = OpenAI()
model = "gpt-4o-mini"

# Just to test if the API is working
random_joke = client.responses.create(
    model="gpt-4o-mini",
    instructions="You are a funny comedian who has to come up with a joke about geometry.",
    input="Tell me a funny, punchy, nerdy joke about geometry!",
).output_text
print(random_joke)
print()

async_client = AsyncOpenAI()

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
Exclude all comments and output only a single generated C++ (templated) function code.
It should be a raw text output, without the ``` around the code block.

The JSON data is provided below, please remember all tasks, and think through the answer carefully before generating the single function for this polyhedron!

"""


async def query_api(prompt):
    response = await async_client.chat.completions.create(
        model=model,
        messages=[{"role":"user", "content":prompt}],
        n=1,
    )
    return response.choices[0].message.content


async def generate_response(polyhedron):
    prompt = (
        f"{base_prompt}\n\n"
        f"Polyhedron Name: {polyhedron['name']}\n"
        f"Constants: {json.dumps(polyhedron['constants'])}\n"
        f"Vertices: {json.dumps(polyhedron['vertices'])}\n\n"
        "Generate the corresponding C++ function."
    )
    response = await query_api(prompt)
    print(f"Generated code for polyhedron: {polyhedron['name']}")
    return response


async def process_category(category, polyhedron_list):
    code = f"// Category: {category}\n\n"
    tasks = [generate_response(poly) for poly in polyhedron_list]
    responses = await asyncio.gather(*tasks)
    for response in responses:
        code += response + "\n\n"
    print(f"Generated code for category: {category}")
    return code


async def main():
    all_generated_code = ""
    with open("../scrape/polyhedra_data.json", "r") as json_file:
        polyhedra = json.load(json_file)
    tasks = [
        process_category(category, polyhedron_list)
        for category, polyhedron_list in polyhedra["Polyhedra"].items()
    ]
    results = await asyncio.gather(*tasks)
    for code in results:
        all_generated_code += code

    output_file = "polyhedra_code.txt"
    with open(output_file, "w") as code_file:
        code_file.write(all_generated_code)
    print(f"Saved polyhedra code to {output_file}")


if __name__ == "__main__":
    asyncio.run(main())
