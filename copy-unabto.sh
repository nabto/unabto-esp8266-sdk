#!/bin/bash
# This script copies the relevant files from the uNabto submodule.

cp -r unabto/src/unabto src/
cp unabto/src/unabto_version.h src/

mkdir -p src/modules/crypto
cp -r unabto/src/modules/crypto/generic src/modules/crypto/

mkdir -p src/platforms
cp unabto/src/platforms/unabto_common_types.h src/platforms/

echo "Relevant files from uNabto submodule copied to src folder."
