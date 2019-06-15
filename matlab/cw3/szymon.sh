poddzwiekowe, niescisliwe
okolo dzwiekowe, ponad dzwiekowe
elektromagnetyzm
liniowa elastycznosc
metoda elementow brzegowych

z kazdej kategorii po dwa zadania

cw3 dir rozpakuj tarem

ls : cw3
cd cw3
5 katalogow w srodku - na kazda kategorie


	NS
cd NS #navire stocks niescisliwe
export UNAME=Szozda
cd data_step
../a.out
#prompt: initialize
mesh
plot
end
solve #50 krokow
twodim #predkosc w kierunku poziomym na drugiej stronie cw3pdf comp1 ux comp=1
tridim
adapth
plot
#save mesh Y
load 1.mesh #mesh 1
plot #sprawdzenie siatki
end #menuglowne
solve
plot
adapth
#patrzymynabledy , plot
end # zapisujemy siatke 1.mesh
load 1.mesh
grid

#zrzut ekranu po trzech adaptacjach , skomentowac obrazek , zaznaczyc jaki comp uzyto, drugi mapa rozkladu cisnienia 2D
#ma wyjsc 20 rysunkow 5zadan x 2cw x 2rys




	CNS
cd../ dataplate1
../a.out
deck
mesh
plot
plot #widac siatke , oplyw nad plyta,
solve
@ #czytanie zamaist z klawiatury czyta z pliku , 300 krokow i zabic
../a.out
deck_r #resume
twodim #widac rozwiazanie
######
comp=8 cisnienie
comp=9 ped poziomy
######
adapth
mesh #elementy podzielone
end #czy zapisac, tak
../a.out
deck_r
solve #300 krokow ,kill
./a.out
deck_r
twodim lub tridim
adapth
mesh, plot #widac siatka podzielona
end #zapisz siatke
..a.out
deck_r
solve
twodim , tridim #zapisz do sprawozdania





	EM
#####
cd EM/datadipol4
../a.out
#frequency 1.3
# 4 dipol
#kat padania 40
mesh
plot
solve
twodim
#### comp=-2 Ey, urojone
tridim #rozumiem 3 adaptacje i jazda???



	LE
######
cd data_frame2d
../a.out
mesh
solve
# 3 zeby wlaczyc rozwiacywanie
#zapyta o rodzaj frame2d
#set pxpy pz 1 2 -6
end
twodim
##duzo wartosci comp
end
14 #opcja do wlaczenia adaptacji
3 # ilosc adaptacji
end
twodim #2 teownik z kolorkami
#comp=109
#przestrzegam przed zadaniami z 6 na koncu , bo sa duze i nie rozwiaze



	BEM
##ostatnia dziedzina cd BEM
cd data_square
../a.out
14
60 #kat padania
33 # jedyna opcja
twodim # widac rozklad pradu na niebieskim obiekcie w kieunku poziomym
comp = -2 #Iy urojny itd
movie
quit