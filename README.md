# Proiect APP

### Echipa: Prunescu Bogdan-Andrei, Dinuta Eduard-Stefan, Bogdan Valentin-Razvan (341C1) 

## Scurta descriere:

Marching Squares este un algoritm de grafica introdus in anii 1980 care poate fi folosit pentru delimitarea contururilor dintr-o imagine. El poate fi folosit pentru a desena linii de altitudine pe harti topografice, temperaturi pe harti termice, puncte de presiune pe harti de camp de presiune, etc.

## Exemplu
<img src="./images/in_mic.png" width="300"/> <img src="./images/out_mic.png" width="300"/> 
## Week 1

- Creare repo github si README
- Implementare cod serial:
    - Complexitate temporala: O(n^2)
    - Complexitate spatiala: O(n^2)
- Realizare teste de diferite dimensiuni si calcularea timpilor de executie:
    - test mic : 2s
    - test mediu : 8s
    - test mare: 17.6s

Specs:
- CPU: 11th Gen Intel(R) Core(TM) i7-11700KF @ 3.60GHz
- Thread(s) per core: 2
- Core(s) per socket: 8
- Socket(s): 1
- Caches (sum of all):      
    - L1d: 384 KiB (8 instances)
    - L1i: 256 KiB (8 instances)
    - L2: 4 MiB (8 instances)
    - L3: 16 MiB (1 instance)
- RAM: 16 GB


### Implementare secventiala

Profiling folosind Vtune:
- test mic:

![Alt small profiling](./images/mic1.png)

![Alt small profiling](./images/mic2.png)

- test mediu

![Alt medium profiling](./images/mediu1.png)

![Alt medium profiling](./images/mediu2.png)

- test mare

![Alt big profiling](./images/mare1.png)

![Alt big profiling](./images/mare2.png)

Variatie timp secvential dupa dimensiunea inputului:

![Alt big profiling](./images/time_input.png)

## Week 2

### Implementare pthreads

Timpi de executie in functie de threaduri:

![Alt big profiling](./images/mic_pthreads.png)


![Alt big profiling](./images/mediu_pthreads.png)


![Alt big profiling](./images/mare_pthreads.png)

Profiling:

- Test mic:
    - 2 threaduri:
        ![Alt big profiling](./images/mic_pthreads_2_1.png)

        ![Alt big profiling](./images/mic_pthreads_2_2.png)
    
    - 4 threaduri:
        ![Alt big profiling](./images/mic_pthreads_4_1.png)

        ![Alt big profiling](./images/mic_pthreads_4_2.png)

    - 8 threaduri:
        ![Alt big profiling](./images/mic_pthreads_8_1.png)

        ![Alt big profiling](./images/mic_pthreads_8_2.png)

    - 16 threaduri:
        ![Alt big profiling](./images/mic_pthreads_16_1.png)

        ![Alt big profiling](./images/mic_pthreads_16_2.png)

- Test mediu:
    - 2 threaduri:
        ![Alt big profiling](./images/mediu_pthreads_2_1.png)

        ![Alt big profiling](./images/mediu_pthreads_2_2.png)
    
    - 4 threaduri:
        ![Alt big profiling](./images/mediu_pthreads_4_1.png)

        ![Alt big profiling](./images/mediu_pthreads_4_2.png)

    - 8 threaduri:
        ![Alt big profiling](./images/mediu_pthreads_8_1.png)

        ![Alt big profiling](./images/mediu_pthreads_8_2.png)

    - 16 threaduri:
        ![Alt big profiling](./images/mediu_pthreads_16_1.png)

        ![Alt big profiling](./images/mediu_pthreads_16_2.png)

- Test mare:
    - 2 threaduri:
        ![Alt big profiling](./images/mare_pthreads_2_1.png)

        ![Alt big profiling](./images/mare_pthreads_2_2.png)
    
    - 4 threaduri:
        ![Alt big profiling](./images/mare_pthreads_4_1.png)

        ![Alt big profiling](./images/mare_pthreads_4_2.png)

    - 8 threaduri:
        ![Alt big profiling](./images/mare_pthreads_8_1.png)

        ![Alt big profiling](./images/mare_pthreads_8_2.png)

    - 16 threaduri:
        ![Alt big profiling](./images/mare_pthreads_16_1.png)

        ![Alt big profiling](./images/mare_pthreads_16_2.png)

