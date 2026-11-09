#!/bin/sh
# Runs a command in the container with this repository at /host, or opens a shell.
# The image is built from docker/Dockerfile on first use.
set -eu

IMAGE=598ape-hw3
docker image inspect "$IMAGE" >/dev/null 2>&1 || "$(dirname "$0")/dockerbuild.sh" "$IMAGE"

# no terminal when stdin is not one
[ -t 0 ] && tty=-it || tty=-i
docker run $tty --rm --user "$(id -u):$(id -g)" --security-opt seccomp=unconfined -v "$(pwd)":/host -w /host "$IMAGE" "${@:-/bin/bash}"
