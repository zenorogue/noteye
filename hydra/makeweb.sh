#!/bin/sh
em++ -DEMS hydra.cpp -std=c++11 -o hydra.js -s EXPORTED_FUNCTIONS="['_doit','_webkey','_eachtick']" -lidbfs.js -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']"
 