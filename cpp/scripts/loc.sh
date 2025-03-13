#!/bin/bash

set -e

cd "$(dirname "${BASH_SOURCE[0]}")/../../"

cloc . --exclude-dir=.idea,build,node_modules,.svelte-kit,package-lock.json
