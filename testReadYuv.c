#include<stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#define DATA_SIZE 1920*1080*2
#define IMG_SIZE_540P 960*540
#define IMG_SIZE_720P 1280*720
#define IMG_SIZE_1080P 1920*1080
#define IMG_SIZE_2K 2560*1440
#define IMG_SIZE_4K 3840*2160

#define BYTE_PIXEL_RATIO1 1
#define BYTE_PIXEL_RATIO2 2
#define BYTE_PIXEL_RATIO3 3
#define BYTE_PIXEL_RATIO4 4
#define BYTE_PIXEL_RATIO6 6

int __yuvTypeJudge(int standard_pic, int pic_size)
{
    // printf("Enter %s, pic_size %d\n", __func__, pic_size);
    int ret = 0;
    int temp_value = pic_size*BYTE_PIXEL_RATIO2;
    // printf("standard_pic :%d, temp_value: %d\n", standard_pic, temp_value);
    if (temp_value == standard_pic*BYTE_PIXEL_RATIO3) {
        printf("yuvType is YUV420\n");
        ret = 1;
    } else if (temp_value == standard_pic*BYTE_PIXEL_RATIO4) {
        printf("yuvType is YUV422\n");
        ret = 1;
    } else if (temp_value == standard_pic*BYTE_PIXEL_RATIO6) {
        printf("yuvType is YUV444\n");
        ret = 1;
    }
    
    return ret;
}

int yuvType(int pic_size)
{
    // printf("Enter %s, pic_size %d\n", __func__, pic_size);
    int ret = 0;
    ret = __yuvTypeJudge(IMG_SIZE_540P, pic_size);
    if (ret) {
        printf("picSize is 540P\n");
        return ret;
    }
    ret = __yuvTypeJudge(IMG_SIZE_720P, pic_size);
    if (ret) {
        printf("picSize is 720P\n");
        return ret;
    }
    ret = __yuvTypeJudge(IMG_SIZE_1080P, pic_size);
    if (ret) {
        printf("picSize is 1080P\n");
        return ret;
    }
    ret = __yuvTypeJudge(IMG_SIZE_2K, pic_size);
    if (ret) {
        printf("picSize is 2K\n");
        return ret;
    }
    ret = __yuvTypeJudge(IMG_SIZE_4K, pic_size);
    if (ret) {
        printf("picSize is 4K\n");
        return ret;
    }

    return ret;
}


int main()
{
    // printf("%d,%d,%d\n", IMG_SIZE_540P, IMG_SIZE_540P*BYTE_PIXEL_RATIO3/BYTE_PIXEL_RATIO2, IMG_SIZE_540P*2, IMG_SIZE_540P*3);  //打印size
    // int size1 = (int) (IMG_SIZE_720P*1.5);
    // printf("%d,%d,%d,%d\n", IMG_SIZE_720P, size1, IMG_SIZE_720P*2, IMG_SIZE_720P*3);  //打印size
    // printf("%d,%d,%d,%d\n", IMG_SIZE_1080P, IMG_SIZE_1080P*BYTE_PIXEL_RATIO3/BYTE_PIXEL_RATIO2, IMG_SIZE_1080P*2, IMG_SIZE_1080P*3);  //打印size
    // printf("%d,%d,%d,%d\n", IMG_SIZE_2K, IMG_SIZE_2K*BYTE_PIXEL_RATIO3/BYTE_PIXEL_RATIO2, IMG_SIZE_2K*2, IMG_SIZE_2K*3);  //打印size
    // printf("%d,%d,%d,%d\n", IMG_SIZE_4K, IMG_SIZE_4K*BYTE_PIXEL_RATIO3/BYTE_PIXEL_RATIO2, IMG_SIZE_4K*2, IMG_SIZE_4K*3);  //打印size
    // printf("Enter %s\n", __func__);
    int nFrame,pic_size;
    int ret = 0;
    FILE * in,*out, *out_y, *reshape1_y, *reshape2_y, *reshape1_yuv; //获取输入序列

    in = fopen("D:\\yst_workplace\\yuv_classTest\\sample_0.yuv","rb+"); //rb 表示读写一个二进制的文件
    out = fopen("D:\\yst_workplace\\yuv_classTest\\copy_sample_0.yuv","wb+"); //用a会导致文件变大，还是用w+
    //若存在该文件，则将要添加的内容放在在文件的末尾 
    if(in == NULL) {
        return -1;
    }
    
    fseek(in,0,SEEK_END); //将文件指针移到文件末尾 
    pic_size = ftell(in); //得到文件尾相对于文件首的位移，即文件的总字节数

    printf("size:%d\n", pic_size);  //打印size
    ret = yuvType(pic_size);

    char* data_buffer = (char*)malloc(pic_size);
    rewind(in); //重置文件指针指向文件头部
    
    fread(data_buffer, 1, pic_size, in);//图像总的字节大小，存储到data_buffer里面

    printf("char_size %d", sizeof(char));
    // printf("%d\n",data_buffer);
    
    fwrite(data_buffer, 1, pic_size, out);//重新写一遍，确认读法无误

/*1、提取y分量并写入新文件*/    
    out_y = fopen("D:\\yst_workplace\\yuv_classTest\\sample0_y.yuv","wb+"); //用a会导致文件变大，还是用w+
    fwrite(data_buffer, 1, IMG_SIZE_1080P, out_y);//重新写一遍，确认读法无误

/*2、只拼接y分量并写入新文件*/    
    reshape1_y = fopen("D:\\yst_workplace\\yuv_classTest\\reshape1_y.yuv","wb+"); //用a会导致文件变大，还是用w+
    char* re1y_buffer = (char*)malloc(IMG_SIZE_1080P*2);
    //Y分量的拼接
    for(int i = 0; i<IMG_SIZE_1080P; i++) {
        *re1y_buffer = *data_buffer;
        *(re1y_buffer + 1920) = *data_buffer;//横向拼接，新图像第0和第1920个像素映射旧图像的第0个像素，以此类推
        data_buffer++;
        re1y_buffer++;
        if (i%1920 == 1919) {
            re1y_buffer += 1920;//完成旧图像的一行拼接后，指针走到下一行
        }
    }

    data_buffer -= IMG_SIZE_1080P;
    re1y_buffer -= IMG_SIZE_1080P*2;

    fwrite(re1y_buffer, 1, IMG_SIZE_1080P*2, reshape1_y);//Y分量图像横向拼接


/*3、拼接YUV并写入新文件*/    
    reshape1_yuv = fopen("D:\\yst_workplace\\yuv_classTest\\reshape1_yuv.yuv","wb+"); //
    char* re1yuv_buffer = (char*)malloc(pic_size*2);
    int t = 0;
    for(int i = 0; i<pic_size; i++) {//IMG_SIZE_1080P换成了pic_size
        *re1yuv_buffer = *data_buffer;
        *(re1yuv_buffer + 1920) = *data_buffer;//横向拼接，新图像第0和第1920个像素映射旧图像的第0个像素，以此类推
        data_buffer++;
        re1yuv_buffer++;
        t++;
        if (i%1920 == 1919) {
          re1yuv_buffer += 1920;//完成旧图像的一行拼接后，指针走到下一行
          t += 1920;
        }
    }

    data_buffer -= pic_size;
    re1yuv_buffer -= pic_size*2;

    fwrite(re1yuv_buffer, 1, pic_size*2, reshape1_yuv);//Y分量图像横向拼接 

    free(re1y_buffer);
    re1y_buffer = NULL;

    free(re1yuv_buffer);
    re1yuv_buffer = NULL;

    free(data_buffer);
    data_buffer = NULL;

    fclose(in);
    fclose(out);
    fclose(out_y);
    fclose(reshape1_y);
    fclose(reshape1_yuv);

    // system("pause");
    return 0;
}