#!/bin/bash


execute(){
	sudo ./a.out -n $console_pts $1
}

console_pts="/dev/pts/1"
c2dir="~szozda/git/javac01/matlab/c2/"

#execute "cd ${c2dir}data_1D"
execute "../a.out"
execute "deck"
execute "mesh"
execute "end"
execute "solve"
execute "errest"

#4 times
execute "mesh"
execute "refine,global"
execute "end"
execute "solve"
execute "errest"

execute "mesh"
execute "enrich,global,order=2"
execute "end"
execute "solve"
execute "errest"

#4 times
execute "mesh"
execute "refine,global"
execute "end"
execute "solve"
execute "errest"

execute "mesh"
execute "enrich,global,order=3"
execute "end"
execute "solve"
execute "errest"

#4 times
execute "mesh"
execute "refine,global"
execute "end"
execute "solve"
execute "errest"