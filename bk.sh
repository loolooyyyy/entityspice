#!/bin/bash

# works for me :shrug

set -eu

basename="entityspice"
dir="${basename}_bk"
bkin="/a/$dir"
basedir="/var/www/ep/sites/all/modules/${basename}"
name=${basename}_$(date +%Y_%m_%d)

function clean() {
	rm -rf "$bkin"
	rm -rf "/a/${name}.tar.xz"
	rm -rf "/a/${name}.tar"
	mkdir "$bkin"
	cd "$bkin"
}

function get() {
	cp -r $basedir/* "$bkin"
	for i in $bkin/*; do
		if [[ -d $i ]]; then
			rm -rf $i/build 
		fi
	done
}

function archive() {
	cd /a/
	tar cf "${name}.tar" "$dir"
	xz -9 "${name}.tar"
}

function upload() {
	gdrive upload "${name}.tar.xz"
}

clean
get
archive
upload

