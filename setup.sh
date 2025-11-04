#!/bin/bash

# Install pre-commit using sudo apt:
echo "Installing pre-commit..."
sudo apt install pre-commit
echo "Finished installing pre-commit..."

# Install clang-format using sudo apt:
echo "Installing clang-format..."
sudo apt install clang-format
echo "Finished installing clang-format..."

# install pre-commit into your git hooks:
echo "Installing pre-commit to git hooks..."
pre-commit install
echo "Finished installing pre-commit to git hooks..."

# Enabling git commit template:
echo "Enabling git commit template..."
git config commit.template .gitmessage
echo "git commit template Enabled..."

echo "Installing cmake..."
sudo apt install cmake
echo "Finished installing cmake..."

sudo apt install libboost-all-dev
sudo apt install curl
curl -fsSL https://www.phidgets.com/downloads/setup_linux | sudo -E bash -
sudo apt install -y libphidget22
sudo apt-get install libphidget22-dev
sudo apt-get install libsdl2-dev
sudo apt install g++
sudo apt-get install libjsoncpp-dev

echo Please be sure to setup "make" if you have yet to do so.
sudo apt install make
echo "Have a nice day :)"
