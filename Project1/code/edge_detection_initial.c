#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#define N 278 // row dimension
#define M 420 // column dimension
#define filename "car_420x278_444.yuv" //use of YUV444 images
#define file_edges "car_420x278_edges.yuv"

/* code for armulator*/
#pragma arm section zidata="ram"
int current_y[N][M];
int current_u[N][M];
int current_v[N][M];


#pragma arm section
int i,j;

void read()
{
  FILE *frame_c;
  if((frame_c=fopen(filename,"rb"))==NULL)
  {printf("current frame doesn't exist\n");exit(-1);}
  for(i=0;i<N;i++){
    for(j=0;j<M;j++)
    {current_y[i][j]=fgetc(frame_c);}}
  for(i=0;i<N;i++){
    for(j=0;j<M;j++)
    {current_u[i][j]=fgetc(frame_c);}}
  for(i=0;i<N;i++){
    for(j=0;j<M;j++)
    {current_v[i][j]=fgetc(frame_c);}}
  fclose(frame_c);
}
void write()
{
  FILE *frame_edges;
  frame_edges=fopen(file_edges,"wb");
  for(i=0;i<N;i++){
    for(j=0;j<M;j++)
    {fputc(current_y[i][j],frame_edges);}}
  for(i=0;i<N;i++){
    for(j=0;j<M;j++)
    {fputc(current_u[i][j],frame_edges);}}
  for(i=0;i<N;i++){
    for(j=0;j<M;j++)
    {fputc(current_v[i][j],frame_edges);}}
  fclose(frame_edges);
}

int Ix[N][M],Iy[N][M];
float rescaled_values[N][M];
int gauss_img[N][M];
float min = 10000,max = 0;// initialize condition variables, later to be used for scaling
int sobel1[3][3]= {{-1, 0, 1},{-2, 0, 2},{-1, 0, 1}},sobel2[3][3] = {{-1, -2, -1},{0, 0, 0},{1, 2, 1}},
gauss[3][3]= {{1, 2, 1},{2, 4, 2},{1, 2, 1}};
float dI[N][M],theta[N][M];

int main() {


  read();
  printf("Create Gaussian Image...\n");
  for (i = 0; i < N; i++)
  {
    for (j = 0; j < M; j++)
    {
      if (i==0) continue; //image boundaries, do nothing (instead of zero padding)
      else if (i==N-1) continue;
      else if (j==0) continue;
      else if (j==M-1) continue;
      else { //convolve gauss filter in grayscale image,grayscale is the Y Channel
      gauss_img[i][j] = current_y[i-1][j-1] * gauss[0][0] + current_y[i-1][j] * gauss[0][1] + current_y[i-1][j+1] * gauss[0][2]
      + current_y[i][j-1] * gauss[1][0] + current_y[i][j] * gauss[1][1] + current_y[i][j+1] * gauss[1][2]
      + current_y[i+1][j-1] * gauss[2][0] + current_y[i+1][j] * gauss[2][1]+ current_y[i+1][j+1] * gauss[2][2];
      }
    }
  }
  printf("Finding gradient image...\n");

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++){
      if (i==0) continue; //oriakes sinthikes, instead of zero pad
      else if (i==N-1)continue;
      else if (j==0) continue;
      else if (j==M-1)continue;
      else //determine gradient in each dimension, then calculate angle and magnitude values
      { // convolve gaussian image with sobel filters
        //Y axis edges
        Ix[i][j] = gauss_img[i-1][j-1] * sobel1[2][2] + gauss_img[i-1][j] * sobel1[2][1] + gauss_img[i-1][j+1] * sobel1[2][0]
          + gauss_img[i][j-1] * sobel1[1][2] + gauss_img[i][j] * sobel1[1][1] + gauss_img[i][j+1] * sobel1[1][0]
          + gauss_img[i+1][j-1] * sobel1[0][2] + gauss_img[i+1][j] * sobel1[0][1] + gauss_img[i+1][j+1] * sobel1[0][0];
        //X axis edges
        Iy[i][j] = gauss_img[i-1][j-1] * sobel2[2][2] + gauss_img[i-1][j] * sobel2[2][1] + gauss_img[i-1][j+1] * sobel2[2][0]
          + gauss_img[i][j-1] * sobel2[1][2] + gauss_img[i][j] * sobel2[1][1] + gauss_img[i][j+1] * sobel2[1][0]
          + gauss_img[i+1][j-1] * sobel2[0][2] + gauss_img[i+1][j] * sobel2[0][1] + gauss_img[i+1][j+1] * sobel2[0][0];
        dI[i][j] = pow((pow(Ix[i][j],2)+pow(Iy[i][j],2)),0.5); // calculate magnitude values
        theta[i][j] = atan2(Ix[i][j], Iy[i][j]); // calculate and round angle values
        theta[i][j] = theta[i][j] * (180/3.14159); // radians to degrees, error using M_MPI in armulator
        if (theta[i][j] < 0) theta[i][j] += 180; // convert all angles to positive angles
        if (dI[i][j] > max) max = dI[i][j];//find max value, later to be used for scaling
        if (dI[i][j] < min) min = dI[i][j];// find min value, later to be used for scaling
    }
  }}
  printf("Scaling & coloring...\n");

  for (i = 0; i < N; i++)
  {
    for (j = 0; j < M; j++)
    {
    rescaled_values[i][j] = ((dI[i][j]-min)/(max-min))*255;//scale the magnitude values,convert to integers
	  current_y[i][j] = rescaled_values[i][j]; // New Y Channel values
    //determine edge direction and color accordingly
	     if (current_y[i][j] > 45){//authaireto orio,45 gia car,30 gia cat,30 gia sunflower
	        if (theta[i][j] >= 0 && theta[i][j] < 22.5){ //horizontal edge
                  current_u[i][j] = 255;//blue UV channel values
                  current_v[i][j] = 107;}
          else if (theta[i][j] >= 157.5 && theta[i][j] <= 180) {//horizontal edge
      	          current_u[i][j] = 255;//blue UV channel values
                  current_v[i][j] = 107;}
          else if (theta[i][j] >= 22.5 && theta[i][j] < 67.5) {//inclined edge
                  current_u[i][j] = 84;//red UV channel values
                  current_v[i][j] = 255;}
          else if (theta[i][j] >= 67.5 && theta[i][j] < 112.5) {//vertical edge
                  current_u[i][j] = 16;//yellow UV channel values
                  current_v[i][j] = 146;}
          else if (theta[i][j] >= 112.5 && theta[i][j] < 157.5) {//inclined edge
                  current_u[i][j] = 43;//green UV channel values
                  current_v[i][j] = 21;}
	        else continue;}
    }
  }
  write();
  return 0;
}
