#!/bin/bash
# This script copies the relevant files from the uNabto submodule.

cp -r unabto/src/unabto src/
cp unabto/src/unabto_version.h src/

mkdir -p src/modules/crypto
cp -r unabto/src/modules/crypto/generic src/modules/crypto/

mkdir -p src/modules/fingerprint_acl
cp unabto/src/modules/fingerprint_acl/fp_acl.*        src/modules/fingerprint_acl/
cp unabto/src/modules/fingerprint_acl/fp_acl_ae.*     src/modules/fingerprint_acl/
cp unabto/src/modules/fingerprint_acl/fp_acl_memory.* src/modules/fingerprint_acl/

mkdir -p src/modules/util
cp unabto/src/modules/util/read_hex.* src/modules/util/

mkdir -p src/platforms
cp unabto/src/platforms/unabto_common_types.h src/platforms/

echo "Relevant files from uNabto submodule copied to src folder."
