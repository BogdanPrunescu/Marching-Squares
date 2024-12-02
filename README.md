# Proiect APP

### Echipa: Prunescu Bogdan-Andrei, Dinuta Eduard-Stefan, Bogdan Valentin-Razvan (341C1) 

## Scurta descriere:

Marching Squares este un algoritm de grafica introdus in anii 1980 care poate fi folosit pentru delimitarea contururilor dintr-o imagine. El poate fi folosit pentru a desena linii de altitudine pe harti topografice, temperaturi pe harti termice, puncte de presiune pe harti de camp de presiune, etc.

## Exemplu
Programul construieste imagini care constituie conturul reliefului pentru diferite altitudini. Prima poza este inputului iar restul sunt calculate folosing sigma = 208, 136 si 61.
<img src="./images/test_image.jpg" width="600"/> <img src="./images/test_208.jpg" width="600"/>
<img src="./images/test_136.jpg" width="600"/> <img src="./images/test_61.jpg" width="600"/>  
## Week 1

- Creare repo github si README
- Implementare cod serial:
    - Complexitate temporala: O(n^2)
    - Complexitate spatiala: O(n^2)
- Realizare teste de diferite dimensiuni si calcularea timpilor de executie:
    - test mic (2048x2048 pixeli) : 2.121s
    - test mediu (4096x4096 pixeli) : 7.631s
    - test mare: (8192x8192 pixeli) 16.292s
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
![Alt small profiling](./images/seq/mic1.png)

![Alt small profiling](./images/seq/mic2.png)

- test mediu

![Alt medium profiling](./images/seq/mediu1.png)

![Alt medium profiling](./images/seq/mediu2.png)

- test mare

![Alt big profiling](./images/seq/mare1.png)

![Alt big profiling](./images/seq/mare2.png)

Variatie timp secvential dupa dimensiunea inputului:

![Alt big profiling](./images/seq/seq_execution_time.png)

## Week 2

### Implementare pthreads

Timpi de executie in functie de threaduri:

![Alt big profiling](./images/pthreads/mic_mare_mediu_pthreads.png)

Profiling:

- Test mic (2048 x 2048):
    - 2 threaduri:

        ![Alt big profiling](./images/pthreads/mic_pthreads_2_1.png)

        ![Alt big profiling](./images/pthreads/mic_pthreads_2_2.png)
    
    Pentru 2 threaduri se observa ca cele 2 threaduri sunt folosite la maxim in cel mai mare procent din timp. 

    - 8 threaduri:

        ![Alt big profiling](./images/pthreads/mic_pthreads_8_1.png)

        ![Alt big profiling](./images/pthreads/mic_pthreads_8_2.png)

    Pentru ca testul este prea mic odata cu cresterea numarului de threaduri nu se observa o eficienta foarte buna utilizarii core-urilor.

    - 16 threaduri:

        ![Alt big profiling](./images/pthreads/mic_pthreads_16_1.png)

        ![Alt big profiling](./images/pthreads/mic_pthreads_16_2.png)

    Deja pentru 16 threaduri sunt folosite toate pentru foarte putin timp, folosing on average 8 threaduri.

- Test mediu (4096 x 4096):

    Testul mediu beneficiaza de o utilizare mai buna a threadurilor, dar 16 threaduri inca sunt prea mult pentru acest task, utilizandu-le complet aproape deloc.

    - 8 threaduri:

        ![Alt big profiling](./images/pthreads/mediu_pthreads_8_1.png)

        ![Alt big profiling](./images/pthreads/mediu_pthreads_8_2.png)

    - 16 threaduri:

        ![Alt big profiling](./images/pthreads/mediu_pthreads_16_1.png)

        ![Alt big profiling](./images/pthreads/mediu_pthreads_16_2.png)

- Test mare (8192 x 8192):

    Testul mare beneficiaza la maxim de paralelism si se observa si o o diferenta in timpul petrecut in functii. Un nou bottleneck este functia de rescale a imaginii.
    
    - 16 threaduri:
    
        ![Alt big profiling](./images/pthreads/mare_pthreads_16_1.png)

        ![Alt big profiling](./images/pthreads/mare_pthreads_16_2.png)
    
    Se poate vedea ca pentru 16 threaduri sunt folosite cea mai mult din timp 16 threaduri.

## Week 3

### Implementare openmp

Din timpii de executie se observa ca openmp este putin mai incet decat pthreads, din cauza apelarii de mai multe ori a directivelor pragma, pe cand la pthreads threadurile sunt create o singura data si folosite pana la terminarea programului.
In plus, am folosit directiva schedule cu parametrul auto, dar prin testarea cu diferiti parametrii, guided s-a dovedit a fi cea mai buna alegere.

Timpi de executie in functie de threaduri:

![Alt big profiling](./images/openmp/mic_mare_mediu_openmp.png)

Profiling:

- Test mic (2048 x 2048):
    - 2 threaduri:

        ![Alt big profiling](./images/openmp/mic_openmp_2_1.png)

        ![Alt big profiling](./images/openmp/mic_openmp_2_2.png)

    - 8 threaduri:

        ![Alt big profiling](./images/openmp/mic_openmp_8_1.png)

        ![Alt big profiling](./images/openmp/mic_openmp_8_2.png)

    - 16 threaduri:

        ![Alt big profiling](./images/openmp/mic_openmp_16_1.png)

        ![Alt big profiling](./images/openmp/mic_openmp_16_2.png)

- Test mediu (4096 x 4096):

    - 8 threaduri:

        ![Alt big profiling](./images/openmp/mediu_openmp_8_1.png)

        ![Alt big profiling](./images/openmp/mediu_openmp_8_2.png)

    - 16 threaduri:

        ![Alt big profiling](./images/openmp/mediu_openmp_16_1.png)

        ![Alt big profiling](./images/openmp/mediu_openmp_16_2.png)

- Test mare (8192 x 8192):
    
    - 16 threaduri:
    
        ![Alt big profiling](./images/openmp/mare_openmp_16_1.png)

        ![Alt big profiling](./images/openmp/mare_openmp_16_2.png)