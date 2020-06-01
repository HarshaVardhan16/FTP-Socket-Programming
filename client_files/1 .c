#include <stdio.h>
#include <assert.h>

int main(){
	int S, H;
 	scanf("%d%d", &S, &H);

	int grid[S][H];

	for(int i = 0; i < S; i++){
		for(int j = 0; j < H; j++){
			scanf("%d", &grid[i][j]);
		}
	}

	int Q;
	scanf("%d", &Q);

	int ques[Q][3];
	for(int i = 0; i < Q; i++){
		for(int j = 0; j < 3; j++){
			scanf ("%d", &ques[i][j]);
		}
	}
	printf("\n");

	assert(S > 0 && S < 21);
	assert(H > 0 && H < 31);
	assert(Q > 0 && Q < 61);
	

	for(int i = 0; i < Q; i++){
		int s = ques[i][1];
		int h = ques[i][2];
		int l = ques[i][0];
		int total = 0;
		assert(s >= 0 && s < S);
		assert(h >= 0 && h < H);
		for(int i = h-l; i <= h+l; i++){
			if(i > -1 && i < H && s-l > -1) total += grid[s-l][i];
		}
		for(int i = h-l; i <= h+l; i++){
			if(i > -1 && i < H && s+l < S) total += grid[s+l][i];
		}
		for(int i = s-l+1; i <= s+l-1; i++){
			if(h-l > -1 && i > -1 && i < S) total += grid[i][h-l];
		}
		for(int i = s-l+1; i <= s+l-1; i++){
			if(h+l < H && i > -1 && i < S) total += grid[i][h+l];
		}
		printf("%d\n", total);
	}	

	
}
