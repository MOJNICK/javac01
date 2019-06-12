#!/bin/bash

set -e
set -x

execute(){
	sudo ./a.out -n $console_dev $1
}

console="pts/3"
console_dev="/dev/$console"

#zadanie 2
execute "../a.out | tee result_szozda"
execute "deck"
execute "mesh"
execute "enrich,global,order=1"
execute "end"
execute "solve"
execute "errest"
execute "adapth"

#4 times
execute "solve"
execute "errest"
execute "adapth"

execute "solve"
execute "errest"
execute "adapth"

execute "solve"
execute "errest"
execute "adapth"

execute "solve"
execute "errest"
execute "adapth"

execute "profile=ONE"
execute "profile=ONE"
execute "profile=ONE"

# sleep 5
# ps -aux | grep "$console" | grep 'a.out' | awk '{print $2}' | xargs kill -9

# execute "grep -i \"TOTAL NDOF\|TOTAL ERRORS\|L2:\|H1:\|MAX:\" ./result_szozda"

# zadanie 1 A B C
# execute "../a.out | tee result_szozda"
# execute "deck"
# execute "mesh"
# execute "enrich,global,order=3"
# execute "end"
# execute "solve"
# execute "errest"

# ##4 times
# execute "mesh"
# execute "refine,global"
# execute "end"
# execute "solve"
# execute "errest"

# execute "mesh"
# execute "refine,global"
# execute "end"
# execute "solve"
# execute "errest"

# execute "mesh"
# execute "refine,global"
# execute "end"
# execute "solve"
# execute "sleep 10"
# execute "errest"

# execute "mesh"
# execute "refine,global"
# execute "end"
# execute "solve"
# execute "errest"
# execute "profile=ONE"

# sleep 5
# ps -aux | grep "$console" | grep 'a.out' | awk '{print $2}' | xargs kill -9

# execute "grep -i \"TOTAL NDOF\|TOTAL ERRORS\|L2:\|H1:\|MAX:\" ./result_szozda"