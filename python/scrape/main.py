import glob
import re
import os
import json
from bs4 import BeautifulSoup, Comment


def extract_constants_from_comment(comment_text):
    constants = {}
    pattern = r'^(?:.*\s)?(\S+)\s*=\s*(.+?)\s*$'
    for line in comment_text.split('\n'):
        match = re.match(pattern, line.strip())
        if match:
            name = match.group(1).strip()
            expression = match.group(2).strip()
            constants[name] = expression
    return constants


def parse_vertices(vertices_value):
    vertex_strings = vertices_value.split(',')
    vertex_tuples = []
    for vertex_string in vertex_strings:
        parts = vertex_string.strip().split()
        if len(parts) == 3:
            vertex_tuples.append(tuple(parts))
    return vertex_tuples


def parse_polyhedra(html_text):
    soup_object = BeautifulSoup(html_text, "html.parser")
    polyhedra_collection = []
    table_cells = soup_object.find_all("td", align="center")
    for table_cell in table_cells:
        anchor_tag = table_cell.find("a")
        if not anchor_tag:
            continue
        polyhedron_name = anchor_tag.text.strip()
        mini_convex_div = table_cell.find("div", {"name":"miniConvex"})
        if not mini_convex_div:
            continue
        comment_text = ""
        comment_nodes = table_cell.find_all(string=lambda text:isinstance(text, Comment))
        for comment_node in comment_nodes:
            if re.search(r'^\s*C\d+\s*=', comment_node, re.MULTILINE):
                comment_text = comment_node
                break
        constants_dictionary = extract_constants_from_comment(comment_text) if comment_text else {}
        vertices_input = mini_convex_div.find("input", {"class":"Vertices"})
        if not vertices_input:
            continue
        vertices_value = vertices_input.get("value", "")
        vertex_tuples = parse_vertices(vertices_value)
        polyhedron_data = {
            "name":polyhedron_name,
            "constants":constants_dictionary,
            "vertices":vertex_tuples
        }
        polyhedra_collection.append(polyhedron_data)
    return polyhedra_collection


def print_polyhedra_info(category, polyhedra_collection):
    print(f"Category: {category}")
    for polyhedron in polyhedra_collection:
        print(f"  Polyhedron: {polyhedron['name']}")
        print(f"    Constants: {', '.join([f'{name} = {expression}' for name, expression in polyhedron['constants'].items()])}")
        print(f"    Vertices ({len(polyhedron['vertices'])}): {polyhedron['vertices']}")

    print("-" * 60)


def scrape_and_build_json(data_folder = "polyhedron_data"):
    categories = {}
    html_file_paths = sorted(
        glob.glob(os.path.join(data_folder, "*.html")),
        key=lambda html_file:int(re.search(r'(\d+)_', os.path.basename(html_file)).group(1))
    )
    for html_file_path in html_file_paths:
        with open(html_file_path, "r", encoding="utf-8") as file_object:
            html_text = file_object.read()
        polyhedra_collection = parse_polyhedra(html_text)
        base_file_name = os.path.basename(html_file_path)
        match = re.search(r'_(.+?)\.html$', base_file_name)
        if match:
            category = match.group(1)
            print_polyhedra_info(category, polyhedra_collection)
            if category not in categories:
                categories[category] = []
            categories[category].extend(polyhedra_collection)
    all_data = {"Polyhedra":categories}
    return all_data


if __name__ == "__main__":
    final_polyhedra_dictionary = scrape_and_build_json("polyhedron_data")
    output_file = "polyhedra_data.json"
    with open(output_file, "w", encoding="utf-8") as json_file:
        json.dump(final_polyhedra_dictionary, json_file, indent=2)
    print(f"Saved polyhedra data to {output_file}")
