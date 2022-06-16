## Wątki - podstawy
Rozważmy obraz monochromatyczny w formacie ASCII PGM (Portable Gray Map) reprezentowany jako macierz o wymiarach nxm, w którym każdy element jest liczbą całkowitą z zakresu od 0 do 255.  Zaimplementuj oparty na wątkach program generujący współbieżnie negatyw obrazu podanego na wejściu. Przez negatyw rozumiemy obraz w którym wartość piksela ij wyznaczana jest jako 255 – p gdzie p to wartość tego piksela w obrazie wejściowym

# Program należy zaimplementować w dwóch wariantach ze względu na podział zadania:

Wariant 1: Każdy wątek wyznacza wartości pikseli obrazu wyjściowego tylko dla tych pikseli obrazu wejściowego które przyjmują wartości z określonego zbioru/przedziału wartości. Zbiór liczb dla wątku można przydzielić w dowolny sposób, ale taki, by każdy wątek dostał inne liczby i zadanie było podzielone równo na wszystkie wątki. (50%)

Wariant 2: Podział blokowy – k-ty wątek oblicza wartości pikseli w pionowym pasku o współrzędnych x-owych w przedziale od (𝑘−1)∗ceil(𝑁/𝑚) do 𝑘∗ceil(𝑁/𝑚)−1, gdzie 𝑁 to szerokość obrazu wejściowego a 𝑚 to liczba stworzonych wątków. (30%)

Program przyjmuje następujące argumenty:

liczbę wątków,
sposób podziału obrazu pomiędzy wątki, t.j. jedną z dwóch opcji: numbers / block
nazwę pliku z wejściowym obrazem,
nazwę pliku wynikowego.
Po wczytaniu danych (wejściowy obraz) wątek główny tworzy tyle nowych wątków, ile zażądano w argumencie wywołania. Utworzone wątki równolegle wyznaczają wartości pikseli obrazu wyjściowego. Każdy stworzony wątek odpowiada za wygenerowanie części tego obrazu. Po wykonaniu obliczeń wątek kończy pracę i zwraca jako wynik (patrz pthread_exit) czas rzeczywisty spędzony na tworzeniu przydzielonej mu części wyjściowego obrazu. 

Po zakończeniu pracy przez wszystkie stworzone wątki, wątek główny zapisuje powstały obraz  do pliku wynikowego i wypisuje na ekranie czas rzeczywisty spędzony na wykonaniu zadania (z dokładnością do mikrosekund). W czasie całkowitym należy uwzględnić narzut związany z utworzeniem i zakończeniem wątków (ale bez czasu operacji wejścia/wyjścia).

Wykonaj pomiary czasu operacji w zależności od:

1.     Stosunku rozmiaru obrazu do liczby wątkow (przy czym uwzględnij przynajmniej następującą liczbę wątkow: 1,2,4,8,16)

2.     metody podziału zadania na podzadania

 Wyniki (czasy dla każdego wątku oraz całkowity czas wykonania zadania w zależności od wariantu) zamieść w pliku Times.txt i dołącz wraz z komentarzem do archiwum z rozwiązaniem zadania (20%).
