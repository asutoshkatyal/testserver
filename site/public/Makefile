#!/usr/bin/make -f

mini:
	htmlcompressor -t html -r ./ -o ./
	htmlcompressor -t xml -r ./ -o ./

check:
	vnu --skip-non-html --verbose ./
