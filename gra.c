#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mpi.h"

#define DOMYSLNA_ITERACJA 64
#define SZEROKOSC_SIATKI 256
#define WYMIAR 16 

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

int main(int argc, char **argv)
{

    int globalna_siatka[256] =
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    //Standardowe zmienne MPI
    int numer_procesu;
    int ID, j;
    int iters = 0;
    int numer_iteracji;

    // Konfiguracja liczby iteracji
    if (argc == 1)
    {
        numer_iteracji = DOMYSLNA_ITERACJA;
    }
    else if (argc == 2)
    {
        numer_iteracji = atoi(argv[1]);
    }
    else
    {
        printf("U¿ywanie: ./Gra_w_zycie <numer_iteracji>\n");
        exit(1);
    }

    // Zmiennne dotyczace wiadomosci
    MPI_Status stat;

    // Konfiguracja MPI
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        printf("MPI_Init error\n");
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numer_procesu); // Ustaw numer procesu
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);

    assert(WYMIAR % numer_procesu == 0);

    // Srodowisko konfiguracji
    int *arr = (int *)malloc(WYMIAR * ((WYMIAR / numer_procesu) + 2) * sizeof(int));
    for (iters = 0; iters < numer_iteracji; iters++)
    {
        //printf("%d %d\n",ID, WYMIAR * ((WYMIAR / numer_procesu) + 2));
        j = WYMIAR;
        for (int i = ID * (SZEROKOSC_SIATKI / numer_procesu); i < (ID + 1) * (SZEROKOSC_SIATKI / numer_procesu); i++)
        {
            arr[j] = globalna_siatka[i];
            // if(ID==1)
            //     printf(" %d %d \n",j,i);
            j++;
        }

        if (numer_procesu != 1)
        {
            
            int incoming_1[WYMIAR];
            int incoming_2[WYMIAR];
            int send_1[WYMIAR];
            int send_2[WYMIAR];
            if (ID % 2 == 0)
            {

                //Pierwsze16
                for (int i = 0; i < WYMIAR; i++)
                {
                    send_1[i] = arr[i + WYMIAR];
                    // printf(" - %d - ",send_1[i]);
                    //printf(" %d %d\n ",i,i+WYMIAR);
                }
                //Pierwszy wiersz do ID-1
                MPI_Ssend(&send_1, WYMIAR, MPI_INT, mod(ID - 1, numer_procesu), 1, MPI_COMM_WORLD);

                //Ostatnie16
                for (int i = 0; i < WYMIAR; i++)
                {
                    send_2[i] = arr[(WYMIAR * (WYMIAR / numer_procesu)) + i];
                    // printf(" %d %d\n ",i,(WYMIAR * (WYMIAR / numer_procesu)) + i);
                }
                //Ostatni wiersz do ID+1
                MPI_Ssend(&send_2, WYMIAR, MPI_INT, mod(ID + 1, numer_procesu), 1, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Recv(&incoming_2, WYMIAR, MPI_INT, mod(ID + 1, numer_procesu), 1, MPI_COMM_WORLD, &stat);
                MPI_Recv(&incoming_1, WYMIAR, MPI_INT, mod(ID - 1, numer_procesu), 1, MPI_COMM_WORLD, &stat);
            }
            if (ID % 2 == 0)
            {
                MPI_Recv(&incoming_2, WYMIAR, MPI_INT, mod(ID + 1, numer_procesu), 1, MPI_COMM_WORLD, &stat);
                MPI_Recv(&incoming_1, WYMIAR, MPI_INT, mod(ID - 1, numer_procesu), 1, MPI_COMM_WORLD, &stat);
            }
            else
            {
                //Pierwsze 16
                for (int i = 0; i < WYMIAR; i++)
                {
                    send_1[i] = arr[i + WYMIAR];
                }
                MPI_Ssend(&send_1, WYMIAR, MPI_INT, mod(ID - 1, numer_procesu), 1, MPI_COMM_WORLD);

                //Ostatnie16
                for (int i = 0; i < WYMIAR; i++)
                {
                    send_2[i] = arr[(WYMIAR * (WYMIAR / numer_procesu)) + i];
                }
                MPI_Ssend(&send_2, WYMIAR, MPI_INT, mod(ID + 1, numer_procesu), 1, MPI_COMM_WORLD);
            }
            for (int i = 0; i < WYMIAR; i++)
            {
                arr[i] = incoming_1[i];
                arr[(WYMIAR * ((WYMIAR / numer_procesu) + 1)) + i] = incoming_2[i];
            }
        }
        else
        {
            for (int i = 0; i < WYMIAR; i++)
            {
                arr[i + SZEROKOSC_SIATKI + WYMIAR] = globalna_siatka[i];
                //printf(" %d %d \n",i + SZEROKOSC_SIATKI+WYMIAR,i);
            }
            for (int i = SZEROKOSC_SIATKI; i < SZEROKOSC_SIATKI + WYMIAR; i++)
            {
                arr[i - SZEROKOSC_SIATKI] = globalna_siatka[i - WYMIAR];
                //printf(" %d %d \n",i - SZEROKOSC_SIATKI,i-WYMIAR);
            }
        }
        //Logika gry
        int * final = (int *)malloc(WYMIAR * ((WYMIAR / numer_procesu)) * sizeof(int));

        for (int k = WYMIAR; k < WYMIAR * ((WYMIAR / numer_procesu) + 1); k++)
        {
            int total_rows = WYMIAR * (WYMIAR / numer_procesu) + 2;
            int r = k / WYMIAR;
            int c = k % WYMIAR;
            int prev_r = mod(r - 1, total_rows);
            int prev_c = mod(c - 1, WYMIAR);
            int next_r = mod(r + 1, total_rows);
            int next_c = mod(c + 1, WYMIAR);

            int count = arr[prev_r * WYMIAR + prev_c] + arr[prev_r * WYMIAR + c] + arr[prev_r * WYMIAR + next_c] + arr[r * WYMIAR + prev_c] + arr[r * WYMIAR + next_c] + arr[next_r * WYMIAR + prev_c] + arr[next_r * WYMIAR + c] + arr[next_r * WYMIAR + next_c];
            if (arr[k] == 1)
            {
                if (count < 2)
                    final[k - WYMIAR] = 0;
                else if (count > 3)
                    final[k - WYMIAR] = 0;
                else
                    final[k - WYMIAR] = 1;
            }
            else
            {
                if (count == 3)
                    final[k - WYMIAR] = 1;
                else
                    final[k - WYMIAR] = 0;
            }
        }

        j = 0;
        for (int i = ID * (SZEROKOSC_SIATKI / numer_procesu); i < (ID + 1) * (SZEROKOSC_SIATKI / numer_procesu); i++)
        {
            globalna_siatka[i] = final[j];
            j++;
        }
        MPI_Gather(final, WYMIAR * (WYMIAR / numer_procesu), MPI_INT, &globalna_siatka, WYMIAR * (WYMIAR / numer_procesu), MPI_INT, 0, MPI_COMM_WORLD);

        // Zaktualizowany stan siatki
        if (ID == 0)
        {
            printf("\nIteracja %d: Siatka:\n", iters);
            for (j = 0; j < SZEROKOSC_SIATKI; j++)
            {
                if (j % WYMIAR == 0)
                {
                    printf("\n");
                }
                printf("%d  ", globalna_siatka[j]);
            }
            printf("\n");
        }
    }

    //Czyszczenie pamieci
    free(arr);
    MPI_Finalize(); // Zakoncz zeby wyjsc
}
