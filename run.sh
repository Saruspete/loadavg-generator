#!/usr/bin/env bash

# Author: Adrien Mahieux
# Twitter: @saruspete
# License: "THE BEER-WARE LICENSE" (Revision 42):
#   As long as you retain this notice you can do whatever you want with this stuff.
#   If we meet some day, and you think this stuff is worth it, you can buy me a beer in return


typeset MYSELF="$(realpath $0)"
typeset MYPATH="${MYSELF%/*}"

set -o nounset -o noclobber
export LC_ALL=C
export PATH="/bin:/sbin:/usr/bin:/usr/sbin:$PATH"
export PS4=' (${BASH_SOURCE##*/}:$LINENO ${FUNCNAME[0]:-main})  '

function die {
	echo >&2 "$@"
	exit 1
}

function sysctlSet {
	typeset sysctl="$1"
	typeset value="$2"

	typeset valCurr="$(sysctl "$sysctl" 2>/dev/null)"
	valCurr="${valCurr##*= }"
	if [[ "$valCurr" != "$value" ]]; then
		sysctl "$sysctl=$value"
	fi
}

function rlimitSet {
	typeset ulim="$1"
	typeset uval="$2"

	typeset valCurr="$(ulimit -$ulim)"
	if [[ "$valCurr" != "$uval" ]]; then
		ulimit -$ulim "$uval"
	fi
}
function loadavgGet1 {
	typeset avg="$(</proc/loadavg)"
	echo "${avg%% *}"
}

# Check for compilation
if ! [[ -x "$MYPATH/clone" ]] || [[ "$MYPATH/clone.c" -nt "$MYPATH/clone" ]]; then
	echo "Compiling clone..."
	cc -static -Wall $MYPATH/clone.c -o $MYPATH/clone || die "Compilation error"
fi


# Check for limits
rlimitSet u unlimited
rlimitSet s unlimited
sysctlSet kernel.pid_max 4194304
sysctlSet kernel.threads-max 4194304


# Run the app
# Will consume a lot of task_struct
taskset -c 1,2 ./clone "${1:-max}"
