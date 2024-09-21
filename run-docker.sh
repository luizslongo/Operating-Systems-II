#!/usr/bin/env bash

if [ -z "$(docker images -q ine5424:latest 2> /dev/null)" ]; then
        cmd="docker build -t ine5424 ."
        echo "running $cmd"
        eval "$cmd"
fi

args=

if [ "$#" -gt 0 ]; then
        args="$@"
else
        args="/bin/fish"
fi

cmd="docker run -ti --rm --name ine5424  --mount=type=bind,src=$(pwd),target=/build ine5424 $args"
echo "running $cmd"
eval "$cmd"
