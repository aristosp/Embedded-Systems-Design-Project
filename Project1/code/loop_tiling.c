#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#define N 278 // row dimension
#define M 420 // column dimension
#define MIN(a,b) (((a)<(b))?(a):(b))
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
    for(j=0;j<M;j=j+5)
    {current_y[i][j]=fgetc(frame_c);
    current_y[i][j+1]=fgetc(frame_c);
    current_y[i][j+2]=fgetc(frame_c);
    current_y[i][j+3]=fgetc(frame_c);
    current_y[i][j+4]=fgetc(frame_c);
    }}

  for(i=0;i<N;i++){
    for(j=0;j<M;j=j+5)
    {current_u[i][j]=fgetc(frame_c);
      current_u[i][j+1]=fgetc(frame_c);
      current_u[i][j+2]=fgetc(frame_c);
      current_u[i][j+3]=fgetc(frame_c);
      current_u[i][j+4]=fgetc(frame_c);
    }}

  for(i=0;i<N;i++){
    for(j=0;j<M;j=j+5)
    {current_v[i][j]=fgetc(frame_c);
      current_v[i][j+1]=fgetc(frame_c);
      current_v[i][j+2]=fgetc(frame_c);
      current_v[i][j+3]=fgetc(frame_c);
      current_v[i][j+4]=fgetc(frame_c);
    }}
  fclose(frame_c);
}
void write()
{
  FILE *frame_edges;
  frame_edges=fopen(file_edges,"wb");
  for(i=0;i<N;i++){
    for(j=0;j<M;j=j+5)
    {fputc(current_y[i][j],frame_edges);
      fputc(current_y[i][j+1],frame_edges);
      fputc(current_y[i][j+2],frame_edges);
      fputc(current_y[i][j+3],frame_edges);
      fputc(current_y[i][j+4],frame_edges);
    }}
  for(i=0;i<N;i++){
    for(j=0;j<M;j=j+5)
    {fputc(current_u[i][j],frame_edges);
      fputc(current_u[i][j+1],frame_edges);
      fputc(current_u[i][j+2],frame_edges);
      fputc(current_u[i][j+3],frame_edges);
      fputc(current_u[i][j+4],frame_edges);
    }}
  for(i=0;i<N;i++){
    for(j=0;j<M;j=j+5)
    {fputc(current_v[i][j],frame_edges);
      fputc(current_v[i][j+1],frame_edges);
      fputc(current_v[i][j+2],frame_edges);
      fputc(current_v[i][j+3],frame_edges);
      fputc(current_v[i][j+4],frame_edges);
    }}
  fclose(frame_edges);
}
int Ix[N][M],Iy[N][M];
int gauss_img[N][M];
void gauss_filter(){
  int gauss[3][3]= {{1, 2, 1},{2, 4, 2},{1, 2, 1}};
  printf("Create Gaussian Image...\n");
  for (i = 0; i < N; i++)
  {
    switch (i) {
      default:{
          for (j = 0; j < M; j=j+3)
          { switch (j) {
            default:{ //convolve gauss filter in grayscale image,grayscale is the Y Channel
              gauss_img[i][j] = current_y[i-1][j-1] * gauss[0][0] + current_y[i-1][j] * gauss[0][1] + current_y[i-1][j+1] * gauss[0][2]
              + current_y[i][j-1] * gauss[1][0] + current_y[i][j] * gauss[1][1] + current_y[i][j+1] * gauss[1][2]
              + current_y[i+1][j-1] * gauss[2][0] + current_y[i+1][j] * gauss[2][1]+ current_y[i+1][j+1] * gauss[2][2];
              gauss_img[i][j+1] = current_y[i-1][j] * gauss[0][0] + current_y[i-1][j+1] * gauss[0][1] + current_y[i-1][j+2] * gauss[0][2]
              + current_y[i][j] * gauss[1][0] + current_y[i][j+1] * gauss[1][1] + current_y[i][j+2] * gauss[1][2]
              + current_y[i+1][j] * gauss[2][0] + current_y[i+1][j+1] * gauss[2][1]+ current_y[i+1][j+2] * gauss[2][2];
              gauss_img[i][j+2] = current_y[i-1][j+1] * gauss[0][0] + current_y[i-1][j+2] * gauss[0][1] + current_y[i-1][j+3] * gauss[0][2]
              + current_y[i][j+1] * gauss[1][0] + current_y[i][j+2] * gauss[1][1] + current_y[i][j+3] * gauss[1][2]
              + current_y[i+1][j+1] * gauss[2][0] + current_y[i+1][j+2] * gauss[2][1]+ current_y[i+1][j+3] * gauss[2][2];
            }
            case 0:continue;
            case M-1:continue;}
    case 0:continue;
    case N-1:continue;}
    }
}}}
float dI[N][M],theta[N][M];
void sobel_filtering(){
int sobel1[3][3]= {{-1, 0, 1},{-2, 0, 2},{-1, 0, 1}},sobel2[3][3] = {{-1, -2, -1},{0, 0, 0},{1, 2, 1}};
    printf("Finding gradient image...\n");
    for (i = 0; i < N; i++) {
      switch (i) {
        default:{ for (j = 0; j < M; j=j+3){
          switch (j) {
            default:{
            //Y axis edges
              Ix[i][j] = gauss_img[i-1][j-1] * sobel1[2][2] + gauss_img[i-1][j] * sobel1[2][1] + gauss_img[i-1][j+1] * sobel1[2][0]
                + gauss_img[i][j-1] * sobel1[1][2] + gauss_img[i][j] * sobel1[1][1] + gauss_img[i][j+1] * sobel1[1][0]
                + gauss_img[i+1][j-1] * sobel1[0][2] + gauss_img[i+1][j] * sobel1[0][1] + gauss_img[i+1][j+1] * sobel1[0][0];
              Ix[i][j+1] = gauss_img[i-1][j] * sobel1[2][2] + gauss_img[i-1][j+1] * sobel1[2][1] + gauss_img[i-1][j+2] * sobel1[2][0]
                  + gauss_img[i][j] * sobel1[1][2] + gauss_img[i][j+1] * sobel1[1][1] + gauss_img[i][j+2] * sobel1[1][0]
                  + gauss_img[i+1][j] * sobel1[0][2] + gauss_img[i+1][j+1] * sobel1[0][1] + gauss_img[i+1][j+2] * sobel1[0][0];
              Ix[i][j+2] = gauss_img[i-1][j+1] * sobel1[2][2] + gauss_img[i-1][j+2] * sobel1[2][1] + gauss_img[i-1][j+3] * sobel1[2][0]
                  + gauss_img[i][j+1] * sobel1[1][2] + gauss_img[i][j+2] * sobel1[1][1] + gauss_img[i][j+3] * sobel1[1][0]
                  + gauss_img[i+1][j+1] * sobel1[0][2] + gauss_img[i+1][j+2] * sobel1[0][1] + gauss_img[i+1][j+3] * sobel1[0][0];
            //X axis edges
              Iy[i][j] = gauss_img[i-1][j-1] * sobel2[2][2] + gauss_img[i-1][j] * sobel2[2][1] + gauss_img[i-1][j+1] * sobel2[2][0]
                + gauss_img[i][j-1] * sobel2[1][2] + gauss_img[i][j] * sobel2[1][1] + gauss_img[i][j+1] * sobel2[1][0]
                + gauss_img[i+1][j-1] * sobel2[0][2] + gauss_img[i+1][j] * sobel2[0][1] + gauss_img[i+1][j+1] * sobel2[0][0];
              Iy[i][j+1] = gauss_img[i-1][j] * sobel2[2][2] + gauss_img[i-1][j+1] * sobel2[2][1] + gauss_img[i-1][j+2] * sobel2[2][0]
                  + gauss_img[i][j] * sobel2[1][2] + gauss_img[i][j+1] * sobel2[1][1] + gauss_img[i][j+2] * sobel2[1][0]
                  + gauss_img[i+1][j] * sobel2[0][2] + gauss_img[i+1][j+1] * sobel2[0][1] + gauss_img[i+1][j+2] * sobel2[0][0];
              Iy[i][j+2] = gauss_img[i-1][j+1] * sobel2[2][2] + gauss_img[i-1][j+2] * sobel2[2][1] + gauss_img[i-1][j+3] * sobel2[2][0]
                  + gauss_img[i][j+1] * sobel2[1][2] + gauss_img[i][j+2] * sobel2[1][1] + gauss_img[i][j+3] * sobel2[1][0]
                  + gauss_img[i+1][j+1] * sobel2[0][2] + gauss_img[i+1][j+2] * sobel2[0][1] + gauss_img[i+1][j+3] * sobel2[0][0];
                }
                case 0:continue;
                case M-1:continue;
        }
      }
        case 0:continue;
        case N-1:continue;}
    }
  }
}

float min = 10000,max = 0;// initialize condition variables, later to be used for scaling
int block = 64;
int w,z;
void magn_theta_calc(){
  printf("Calculating magnitude and theta\n");
  for (i = 0; i<N;i+=2) {//fissioned loop
    for (j = 0; j<M; j+=2){
      for (z = i; z<MIN(i+2,N);z++){
        for (w = j; w<MIN(j+2,M);w++){
      dI[z][w] = sqrt((Ix[z][w] * Ix[z][w]) + (Iy[z][w]*Iy[z][w])); // calculate magnitude values.
      theta[z][w] = atan2(Ix[z][w], Iy[z][w]) * (180/3.14159); // calculate and convert  angle values to degrees
      if (theta[z][w] < 0) theta[z][w] += 180; // convert all angles to positive angles
      if (dI[z][w] > max) max = dI[z][w];//find max value, later to be used for scaling
      else if (dI[z][w] < min) min = dI[z][w];
      else continue;}
    }}
}}
void scale() {
  printf("Scaling\n");
  for (i = 0; i<N;i+=2) {//fissioned loop
    for (j = 0; j<M; j+=2){
      for (z = i; z<MIN(i+2,N);z++){
        for (w = j; w<MIN(j+2,M);w++){
  current_y[z][w] = ((dI[z][w]-min)/(max-min))*255; // New Y Channel values//scale the magnitude values,convert to integers
    }
  }
}}}
void color(){
  printf("Coloring...\n");
  for (i = 0; i<N;i+=2) {//fissioned loop
    for (j = 0; j<M; j+=2){
      for (z = i; z<MIN(i+2,N);z++){
        for (w = j; w<MIN(j+2,M);w++){
    //determine edge direction and color accordingly
	     if (current_y[z][w] > 45){//authaireto orio,45 gia car,30 gia cat,30 gia sunflower
	        if (theta[z][w] >= 0 && theta[z][w] < 22.5){ //horizontal edge
                  current_u[z][w] = 255;//blue UV channel values
                  current_v[z][w] = 107;}
          else if (theta[z][w] >= 157.5 && theta[i][w] <= 180) {//horizontal edge
      	          current_u[z][w] = 255;//blue UV channel values
                  current_v[z][w] = 107;}
          else if (theta[z][w] >= 22.5 && theta[i][w] < 67.5) {//inclined edge
                  current_u[z][w] = 84;//red UV channel values
                  current_v[z][w] = 255;}
          else if (theta[z][w] >= 67.5 && theta[i][w] < 112.5) {//vertical edge
                  current_u[z][w] = 16;//yellow UV channel values
                  current_v[z][w] = 146;}
          else  { // last possible outcome
                  current_u[z][w] = 43;//inclined edge
                  current_v[z][w] = 21;//green UV channel values
                }
    }
  }
}
}}}
int main() {
  read();
  gauss_filter();
  sobel_filtering();
  magn_theta_calc();
  scale();
  color();
  write();
  return 0;
}
