import argparse
from pathlib import Path

import yarn_spinner_pb2 as yarn_pb2

parser = argparse.ArgumentParser()
parser.add_argument("binary_file_path", help="Path to the binary file containing the YARN program")
args = parser.parse_args()

# Path to the binary file
binary_file_path = Path(args.binary_file_path)

# Load the binary content
with open(binary_file_path, "rb") as f:
    binary_content = f.read()

# Parse the binary content as a Program message
program = yarn_pb2.Program()
program.ParseFromString(binary_content)

# Access the decoded data
print("Program Name:", program.name)


# Iterate over nodes
for node_name, node in program.nodes.items():
    print("==============================")
    print(node_name)

    for idx, inst in enumerate(node.instructions):
        cmd = str(inst).splitlines()
        print(f'{idx}, instruction {{ {cmd} }}')

    print(node.labels)

