# Proiect APP

### Echipa: Prunescu Bogdan-Andrei, Dinuta Eduard-Stefan, Bogdan Valentin-Razvan (341C1) 

## Scurta descriere:

Marching Squares este un algoritm de grafica introdus in anii 1980 care poate fi folosit pentru delimitarea contururilor dintr-o imagine. El poate fi folosit pentru a desena linii de altitudine pe harti topografice, temperaturi pe harti termice, puncte de presiune pe harti de camp de presiune, etc.

## Week 1

- Creare repo github si README
- Implementare cod serial:
    - Complexitate temporala: O(n^2)
    - Complexitate spatiala: O(n^2)
- Realizare teste de diferite dimensiuni si calcularea timpilor de executie:
    - test mic : 2s
    - test mediu : 7.8s
    - test mare: 14.4s

![Alt times](./images/times.png)

- Profiling folosind Vtune:
    - test mic:

    ![Alt small profiling](./images/mic1.png)

    ![Alt small profiling](./images/mic2.png)

    - test mediu

    ![Alt medium profiling](./images/mediu1.png)

    ![Alt medium profiling](./images/mediu2.png)

    - test mare

    ![Alt big profiling](./images/mare1.png)

    ![Alt big profiling](./images/mare2.png)
