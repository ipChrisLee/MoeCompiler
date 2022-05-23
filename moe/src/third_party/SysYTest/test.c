int ar[3][3];
int fun(int a[][3]){
	return a[1][2]+a[0][1]+a[2][0]; 
}
int main(){
	return fun(ar);
}
