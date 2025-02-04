#!/bin/bash

# Install pre-commit using pip:
echo "Installing pre-commit..."
pip install pre-commit
echo "Finished installing pre-commit..."

# install pre-commit into your git hooks:
echo "Installing pre-commit to git hooks..."
pre-commit install
echo "Finished installing pre-commit to git hooks..."

# Enabling git commit template:
echo "Enabling git commit template..."
git config commit.template .gitmessage
echo "git commit template Enabled..."

echo Please be sure to setup "make" if you have yet to do so.
echo "Have a nice day :)"
