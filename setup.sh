#!/bin/bash

sudo cp -f ./bin/clipper /bin/
sudo chmod +x /bin/clipper
sudo chmod 701 /bin/clipper

cp -f ./data/clipper.desktop ~/.local/share/applications/

if [ -d "~/.local/share/clipper" ]; then
    mkdir ~/.local/share/clipper/
    echo "Creating local dir"
else
    echo "Dir already exists"
fi

cp -f -r ./data ~/.local/share/clipper/
cp -f -r ./ui ~/.local/share/clipper/
