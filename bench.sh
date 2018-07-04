seq 100000 | parallel -n0 -j10000 "curl localhost:10000"
