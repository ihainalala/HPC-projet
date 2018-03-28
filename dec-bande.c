#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define NB_COLS 4
#define NB_LIGNES 10
#define LARGEUR_BANDE 2

static void afficherLigne(int tab[], int t);

int main(int argc, char *argv[]) {
	int rang, nb_instances;
	int id_maitre = 0;
	int nb_esclaves;
	int tab_entier[NB_LIGNES][NB_COLS] = {0};

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_instances);
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);

    nb_esclaves = nb_instances - 1;

	for (int iLigne = 0; iLigne < NB_LIGNES; iLigne++) {
		if (rang == 0) {
            // init de la portion de tableau a envoye a chaque proc
			printf("Traitement iLigne n° %d\n", iLigne);

			for (int iEsclave = 1; iEsclave <= nb_esclaves; iEsclave++) {
				int tab[LARGEUR_BANDE];
                MPI_Status status;

				for (int iCol = 0; iCol < LARGEUR_BANDE; iCol++) {
					tab[iCol] = tab_entier[iLigne][LARGEUR_BANDE * (iEsclave - 1) + iCol];
				}

				MPI_Send(tab, LARGEUR_BANDE, MPI_INT, iEsclave, 0, MPI_COMM_WORLD);
				MPI_Recv(tab, LARGEUR_BANDE, MPI_INT, iEsclave, 0, MPI_COMM_WORLD, &status);

				printf("Je suis %d et j'ai recu %d et %d de proc n°%d\n", rang, tab[0], tab[1], iEsclave);

				// modif de tab_entier
				for (int iCol = 0; iCol< LARGEUR_BANDE; iCol++) {
                    tab_entier[iLigne][LARGEUR_BANDE * (iEsclave - 1) + iCol] = tab[iCol];
				}
			}

			afficherLigne(tab_entier[iLigne], NB_COLS);
		} else {
			int tab[LARGEUR_BANDE];
            MPI_Status status;

			MPI_Recv(tab, LARGEUR_BANDE, MPI_INT, id_maitre, 0, MPI_COMM_WORLD, &status);

			// modif de ces valeurs
			for (int iCol = 0; iCol < LARGEUR_BANDE; iCol++) {
				tab[iCol] = rang;
			}

			// envoi des modifs
			MPI_Send(tab, LARGEUR_BANDE, MPI_INT, id_maitre, 0, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	return 0;
}

void afficherLigne(int tab[], int nbCols) {
	for (int i = 0; i < nbCols; i++) {
		printf("%d |", tab[i]);
	}

	printf("\n");
}

