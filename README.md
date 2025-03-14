# Parallel-Inverted-Index

## Descriere

Acest proiect presupune calculul paralel al unui index inversat utilizând paradigma **Map-Reduce**. Programul este scris în limbajul **C++** folosind biblioteca **Pthreads** pentru gestionarea firelor de execuție.

### Scopul proiectului

Scopul este de a construi un index inversat pentru o colecție de fișiere de intrare, care să indice toate cuvintele diferite din fișiere și fișierele în care acestea apar. Programul utilizează două tipuri de operații principale:
1. **Map** - Realizează extragerea cuvintelor din fișiere.
2. **Reduce** - Agregă rezultatele și sortează cuvintele în fișiere de ieșire în funcție de prima literă.

---

## Paradigma Map-Reduce

1. **Map**:
   - Fiecare thread de tip **Mapper** deschide un fișier de intrare, extrage cuvintele (convertite în litere mici și fără caractere speciale) și generează o listă de perechi `{cuvânt, ID fișier}`.
   - Alocarea fișierelor este dinamică pentru a asigura o distribuție echilibrată între thread-uri.

2. **Reduce**:
   - Fiecare thread de tip **Reducer** colectează rezultatele mapării, le grupează după prima literă a cuvântului și le sortează descrescător în funcție de numărul de fișiere în care apar.
   - Se generează câte un fișier de ieșire pentru fiecare literă a alfabetului (`a.txt`, `b.txt`, etc.).

---

## Compilare

Programul se compilează utilizând comanda:

```
make build
```

Acest lucru va genera un executabil numit `proiect`.

Pentru a curăța fișierele generate la compilare, folosiți comanda:

```
make clean
```

---

## Rulare

Pentru a rula programul, folosiți următoarea comandă:

```
./proiect <numar_mapperi> <numar_reduceri> <fisier_intrare>
```


## Formatul fișierului de intrare

Fișierul de intrare trebuie să conțină numărul de fișiere de procesat pe prima linie, urmat de numele fiecărui fișier pe liniile următoare:

```
3
file1.txt
file2.txt
file3.txt
```

---

## Formatul fișierelor de ieșire

Fiecare literă a alfabetului are asociat un fișier (`a.txt`, `b.txt`, ...). Formatul fiecărui fișier este următorul:

```
cuvant:[ID_fisier1 ID_fisier2 ...]
```

Cuvintele sunt sortate descrescător după numărul de fișiere în care apar, iar în caz de egalitate, alfabetic.

---

## Sincronizare

Programul folosește următoarele mecanisme de sincronizare:
- **pthread_mutex_t** pentru accesul exclusiv la variabilele partajate.
- **pthread_barrier_t** pentru sincronizarea între etapele Map și Reduce.

---


