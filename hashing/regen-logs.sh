#!/bin/bash

make std.compile dshash.compile dshash_wrapped.compile

for t in std dshash dshash_wrapped; do
	./$t books.txt 100 > $t.new.log
done
