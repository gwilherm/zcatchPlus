#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
. $DIR/function_variable_definitions.sh --source-only

check_build_bam
update_source


if [[ $1 == "-c" ]]; then
    clean_previous_build
fi
retrieve_cores
build_debug_server
start_server_debugging









