/*****************************************************************************
* Copyright 2014 Katholieke Universiteit Leuven
*
* Use of this software is governed by the GNU LGPLv3.0 license
*
* Written by Ruan de Clercq, Sujoy Sinha Roy,
* Frederik Vercauteren, and Ingrid Verbauwhede
* Computer Security and Industrial Cryptography (COSIC)
* K.U.Leuven, Departement Electrical Engineering,
* Celestijnenlaan 200A, B-3001 Leuven, Belgium
****************************************************************************/

#include "global.h"
#include "lwe.h"
#include "stdio.h"

int compare_array(uint32_t * a, uint32_t * b)
{
	int i;
	for (i=0; i<M; i++)
	{
		if (a[i]!=b[i])
		{
			return 0;
		}
	}
	return 1;
}

void dump_array(uint32_t * a)
{
	int i;
	for (i=0; i<M; i++)
	{
		printf("%d,",a[i]);
	}
}

void main()
{
	int i;
	int j;
	int res;
	uint32_t message[M],a[M],p[M],r2[M],c1[M],c2[M],large1[M],large2[M],a_0[M/2],a_1[M/2];

/*
	printf("knuth_yao: ");
	res = 1;
	for (i=0; i<32767; i++)
	{
		uint32_t rnd,num1,num2;
		//Test knuth-yao
		srand(i*i);
		//knuth_yao_basic_array(large2);
		rnd = rand();
		num1=knuth_yao_basic_single_number(&rnd);

		srand(i*i);
		//knuth_yao_lut_optimized_array(large1);
		rnd=rand();
		num2=knuth_yao_single_number_lut_optimized(&rnd);

		if (num1!=num2)
		{
			res=0;
			break;
		}
	}
	if (res==0)
		printf("BAD!\n");
	else
		printf("OK!\n");*/

	printf("Fwd/Inv_ntt2: ");
	res=1;
	res = 1;
	for (i=0; (i<1000) && (res==1); i++)
	{
		srand(i);
		if (i==0)
		{
			for (j=0; j<M; j++)
			{
				large1[j]=1;
			}
		}
		else
		{
			for (j=0; j<M; j++)
				large1[j]=rand()%2;
		}

		for (j=0; j<M; j++)
		{
			large2[j]=large1[j];
		}

		fwd_ntt2(large2);
		rearrange2(large2);
		inv_ntt2(large2);


		for (j=0; j<M; j++)
		{
			if (large2[j]!=large1[j])
			{
				printf("i=%d,j=%d",i,j);
				printf("\nlarge1:");
				dump_array(large1);
				printf("\nlarge2:");
				dump_array(large2);
				res=0;
				break;
			}
		}
	}
	if (res==0)
		printf("FAIL\n");
	else
		printf("PASS\n");


	int fail=0;
	printf("Enc/Dec: ");
	/*This test procedure tries to determine whether the cryptosystem works.
	 * It does the following tasks:
	 * 1. Generate a random message
	 * 2. Generate a secret and public key pair
	 * 3. Encrypt the message
	 * 4. Decrypt the encrypted message
	 * 5. Check if the decrypted message matches the original
	*/
	for(i=0; i<1000; i++)
	{
		message_gen2(message);
		bitreverse2(message);
		key_gen2(a,p,r2);

		RLWE_enc2(a,c1,c2,message,p);
		RLWE_dec2(c1,c2,r2);

		for(j=0; j<M; j++)
		{
			if ((c1[j]>QBY4) && (c1[j]<QBY4_TIMES3))
				c1[j] = 1;
			else
				c1[j] = 0;
		}

		//Determine if the decryption was correct:
		bitreverse2(message);

		rearrange_decrypted_message(c1,large1);
		for (j=0; j<M; j++)
		{
			if (message[j]!=large1[j])
			{
				fail=1;
				break;
			}
		}
		if (fail==1)
			break;
	}
	if (fail==1)
		printf("FAIL\n");
	else
		printf("PASS\n");
}
