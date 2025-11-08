#!/bin/bash

# Install pre-commit using sudo apt:
echo "Installing pre-commit..."
sudo apt install -y pre-commit
echo "Finished installing pre-commit..."

# Install clang-format using sudo apt:
echo "Installing clang-format..."
sudo apt install -y clang-format
echo "Finished installing clang-format..."

# install pre-commit into your git hooks:
echo "Installing pre-commit to git hooks..."
pre-commit install
echo "Finished installing pre-commit to git hooks..."

# Enabling git commit template:
echo "Enabling git commit template..."
git config commit.template .gitmessage
echo "git commit template Enabled..."

echo "Installing curl, cmake, & make..."
sudo apt install -y curl cmake make
echo "Finished installing curl, cmake, & make..."

curl -fsSL https://www.phidgets.com/downloads/setup_linux | sudo -E bash -
sudo apt install -y g++ libboost-all-dev libphidget22 libphidget22-dev libsdl2-dev libjsoncpp-dev 

echo "Have a nice day :)"
