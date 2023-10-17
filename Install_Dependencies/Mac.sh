#!/bin/sh

# Check if Homebrew is already installed
if command -v brew &> /dev/null; then
    echo "Homebrew is already installed. Updating..."
    brew update
else
    # Install Homebrew if it's not installed
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Check if the path already exists in .zprofile
if ! grep -q '/opt/homebrew/bin/brew shellenv' "/Users/userk/.zprofile"; then
    # If it doesn't exist, add it
    echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> "/Users/userk/.zprofile"
fi

# Activate Homebrew environment
eval "$(/opt/homebrew/bin/brew shellenv)"

# Install the desired packages
brew install boost
brew install tbb
brew install c-blosc
