#!/bin/bash

[ -z "$1" ] && echo "Usage: $0 <API_KEY>" && exit 1

echo "export OPENAI_API_KEY=\"$1\"" >> ~/.bashrc && source ~/.bashrc

echo "API key set in ~/.bashrc"
