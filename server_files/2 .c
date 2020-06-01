#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(){
	srand(time(0));
	int size;
	scanf("%d", &size);
	
	int arr[size];
	for(int i = 0; i < size; i++){
		arr[i] = rand()%2;
		printf("%d ", arr[i]);
	}
	printf("\n");
	
	int zeroCount = 0;
	int oneCount = 0;

	for(int i = 0; i < size; i++){
		if(arr[i] == 0) zeroCount++;
		if(arr[i] == 1) oneCount++;
	}
	
	int swap = size;
	for(int i = 0; i <= zeroCount; i++){
		oneCount = 0;
		for(int j = i; j < size - zeroCount + i; j++){
			if(arr[j] == 0) oneCount++;
		}

		if(oneCount < swap) swap = oneCount;
	}

	printf("minimum number of swaps = %d\n", swap);
		
}
