#include "phash.h"
#include <FreeImage.h>  
#include <sys/stat.h>  
#include <sys/types.h>
#include <cstdio>  
#include <cstdlib>  


IplImage* FIBITMAP_2_IplImage( FIBITMAP* fiBmp, const FREE_IMAGE_FORMAT& fif)
// 将解码的图像数据封装成opencv下的IplImage格式
{
   if(NULL == fiBmp || FIF_GIF != fif) return NULL;
      int width  = FreeImage_GetWidth(fiBmp);
      int height = FreeImage_GetHeight(fiBmp);
      if(width <= 0 || height <= 0)
          return NULL;
      RGBQUAD* ptrPalette = FreeImage_GetPalette(fiBmp);
      BYTE intens;
      BYTE* pIntensity = &intens;
      IplImage* pImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
      pImg->origin = 1;
      for (int i = 0; i < height; i++)
      {
          char* ptrDataLine = pImg->imageData + i * pImg->widthStep;
          for(int j = 0; j < width; j ++)
          {
              FreeImage_GetPixelIndex(fiBmp, j , i, pIntensity);
              ptrDataLine[3*j] = ptrPalette[intens].rgbBlue;
              ptrDataLine[3*j+1] = ptrPalette[intens].rgbGreen;
              ptrDataLine[3*j+2] = ptrPalette[intens].rgbRed;
          }
      }
      return pImg;
}

IplImage* Gif2IplImage(char* fileName)
{
    FreeImage_Initialise(); // 初始化freeimage，一定要FreeImage_DeInitialise()

    struct stat file_stat;  
    int length = 0;  

    length = stat(fileName, &file_stat);  
    BYTE * bytes = (BYTE*)malloc(file_stat.st_size * sizeof(BYTE));  
    FILE * fin = fopen(fileName, "rb" );  
    if (fin) 
    { 
        fread( bytes, sizeof(BYTE),file_stat.st_size, fin); 
        fclose(fin);  
        FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)bytes, file_stat.st_size);
        if(NULL == memory)
        {
            FreeImage_DeInitialise();
            return NULL;
        }

        FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(memory);
        if(FIF_UNKNOWN == fif)
        {
            fprintf(stderr, "Error in Gif2IplImage: is unknow image type/n");
            if(NULL != memory) FreeImage_CloseMemory(memory);
                FreeImage_DeInitialise();
            return NULL;
        }
        else if(FIF_GIF != fif) // 不是gif文件不处理
        {
            if(NULL != memory) FreeImage_CloseMemory(memory);
                FreeImage_DeInitialise();
            return NULL;
        }
        FIMULTIBITMAP* fiBmps = FreeImage_LoadMultiBitmapFromMemory(fif, memory, GIF_DEFAULT);// gif文件解码
        if(NULL == fiBmps)
        {
            if(NULL != memory) FreeImage_CloseMemory(memory);
                FreeImage_DeInitialise();
            return NULL;
        }
  
        FIBITMAP* fiBmp = FreeImage_LockPage(fiBmps, 0);
        if(fiBmp)
        {
             IplImage* pImg =  FIBITMAP_2_IplImage(fiBmp, fif);// 将解码的图像数据封装成opencv下的IplImage格式
             FreeImage_UnlockPage(fiBmps, fiBmp, false); 
             if(NULL == pImg)
             {
                 if(NULL != memory) FreeImage_CloseMemory(memory);
                 if(NULL != fiBmps) FreeImage_CloseMultiBitmap(fiBmps, GIF_DEFAULT);
                     FreeImage_DeInitialise();
                 return NULL;
             }
             if(NULL != memory) FreeImage_CloseMemory(memory);
             if(NULL != fiBmps) FreeImage_CloseMultiBitmap(fiBmps, GIF_DEFAULT);
                 FreeImage_DeInitialise();
             return pImg;
        }
        return NULL;
    }
}

