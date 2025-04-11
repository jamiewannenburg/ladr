#!/usr/bin/env python3

import os
import sys
import re
import networkx as nx
import matplotlib.pyplot as plt
from pathlib import Path

BINDINGS_PATH = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "python_bindings")

def find_header_files(root_dir):
    """Find all .h files in the directory tree."""
    header_files = []
    for root, _, files in os.walk(root_dir):
        for file in files:
            # ignore files in folders starting with a dot or underscore
            if file.endswith('.h') and not '.' in root and not '_' in root:
                header_files.append(os.path.join(root, file))
    return header_files

def parse_includes(file_path):
    """Parse a header file and return a list of included files."""
    includes = []
    include_pattern = re.compile(r'#include\s+["<]([^">]+)[">]')
    
    try:
        with open(file_path, 'r') as f:
            content = f.read()
            includes = include_pattern.findall(content)
            # remove standard library includes
            includes = [inc for inc in includes if not inc.startswith('std')]
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
    
    return includes

def check_python_binding(header_path):
    """Check if there is a corresponding Python binding file for the given header file.
    
    Args:
        header_path (str): Path to the header file
        
    Returns:
        tuple: (bool, str) - (has_binding, binding_path)
            has_binding: True if a binding file exists
            binding_path: Path to the binding file if it exists, None otherwise
    """
    # Get the base name of the header file without extension
    header_name = os.path.splitext(os.path.basename(header_path))[0]
    
    # Construct potential binding file names
    potential_names = [
        f"{header_name}_bindings.cpp",
        f"{header_name}.cpp"
    ]
    
    # Check in the ladr_bindings directory
    bindings_dir = os.path.join(BINDINGS_PATH, "ladr_bindings")
    for name in potential_names:
        binding_path = os.path.join(bindings_dir, name)
        if os.path.exists(binding_path):
            return True, binding_path
    
    return False, None

def build_include_graph(root_dir):
    """Build a graph of header file dependencies."""
    G = nx.DiGraph()
    header_files = find_header_files(root_dir)
    
    for header in header_files:
        rel_path = os.path.relpath(header, root_dir)
        G.add_node(rel_path)
        
        includes = parse_includes(header)
        for inc in includes:
            # Try to find the actual file path
            inc_path = None
            if inc.startswith('ladr/'):
                inc_path = inc
            else:
                # Search for the include file in the directory tree
                for h in header_files:
                    if h.endswith(inc):
                        inc_path = os.path.relpath(h, root_dir)
                        break
            
            if inc_path:
                G.add_edge(rel_path, inc_path)
    
    return G

def visualize_graph(G, output_file='include_graph.png'):
    """Visualize the graph and save it to a file."""
    plt.figure(figsize=(20, 20))
    pos = nx.spring_layout(G, k=1, iterations=50)
    nx.draw(G, pos, with_labels=True, node_size=2000, node_color='lightblue',
            font_size=8, font_weight='bold', arrows=True)
    plt.savefig(output_file, format='png', bbox_inches='tight')
    plt.close()

def main(root_dir):
    print(f"Analyzing header files in {root_dir}...")
    
    G = build_include_graph(root_dir)
    
    # Print basic statistics
    print(f"\nGraph Statistics:")
    print(f"Number of nodes (header files): {G.number_of_nodes()}")
    print(f"Number of edges (include relationships): {G.number_of_edges()}")
    
    # Find and print header files with lest dependencies
    print("\nTop 20 header files with least dependencies:")
    in_degrees = sorted(G.in_degree(), key=lambda x: x[1], reverse=False)
    i=0
    for node, degree in in_degrees:
        has_binding, binding_path = check_python_binding(os.path.join(root_dir, node))
        binding_status = f"Binding: {binding_path}" if has_binding else "No binding"
        print(f"{node}: {degree} dependencies - {binding_status}")
        if i==20:
            break
        if not has_binding:
            i+=1

    # Find and high importance header files with little dependencies
    print("\nTop 20 header files with hight importance and little dependencies:")
    nodes = sorted(G.nodes(), key=lambda x: (G.in_degree(x)+1)/(G.out_degree(x)+1), reverse=True)
    i=0
    for node in nodes:
        has_binding, binding_path = check_python_binding(os.path.join(root_dir, node))
        binding_status = f"Binding: {binding_path}" if has_binding else "No binding"
        print(f"{node}: {G.in_degree(node)} dependencies -> {G.out_degree(node)} - {binding_status}")
        if i==20:
            break
        if not has_binding:
            i+=1

    # Print the 20 nodes with the highest PageRank
    print("\nTop 20 nodes with the highest PageRank:")
    pagerank = nx.pagerank(G)
    pagerank_sorted = sorted(pagerank.items(), key=lambda x: x[1], reverse=True)
    i=0
    for node, pr in pagerank_sorted:
        has_binding, binding_path = check_python_binding(os.path.join(root_dir, node))
        binding_status = f"Binding: {binding_path}" if has_binding else "No binding"
        print(f"{node}: {pr} - {binding_status}")
        if i==20:
            break
        if not has_binding:
            i+=1
    
    # Generate visualization
    print("\nGenerating visualization...")
    visualize_graph(G)
    print(f"Graph visualization saved as include_graph.png")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        root_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    else:
        root_dir = sys.argv[1]
    main(root_dir) 