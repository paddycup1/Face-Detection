#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>

using namespace cv;
using namespace std;

#define WIDTH 24
#define HEIGHT 24

int main(void)
{
	Mat SrcImg, newSrcImg;
	char srcName[256] = { 0 }, dstName[256] = { 0 }, stageDirName[256] = "F:\\not_face\\stage00", tmpStr[256] = { 0 }, sysStageDir[256] = { 0 }, dstNum[256] = { 0 };
	char sysMove[256] = { 0 }, sysRename[256] = { 0 };
	FILE *config, *config_imgBackup;
	int dstCount[1000] = { 0 }, stageNum = 1, endFlag = 0, imgBackupNum = 0;
	DIR *dir = NULL, *tmpDir = NULL;
	dirent *entry = NULL;
	
	if (fopen_s(&config_imgBackup, "F:\\config_imgBackup.txt", "r+") != 0){
		fopen_s(&config_imgBackup, "F:\\config_imgBackup.txt", "w+");
		fclose(config_imgBackup);
		fopen_s(&config_imgBackup, "F:\\config_imgBackup.txt", "r+");
	}
	if (fscanf_s(config_imgBackup, "%d", &imgBackupNum) == EOF){
		fprintf_s(config_imgBackup, "0");
	}
	
	if (fopen_s(&config, "F:\\config.txt", "r+") != 0){
		fopen_s(&config, "F:\\config.txt", "w+");
		fclose(config);
		fopen_s(&config, "F:\\config.txt", "r+");
	}
	if (fscanf_s(config, "%d", &stageNum) == EOF){
		fprintf_s(config, "1\n0\n");
	}
	for (int i = 0; i < stageNum; i++)
		fscanf_s(config, "%d", &dstCount[i]);


	Size size = Size(WIDTH, HEIGHT), size2 = Size(WIDTH * 5, HEIGHT * 5);

	if ((dir = opendir("F:\\not_face")) == NULL)
		system("mkdir F:\\not_face");
	closedir(dir);
	if ((dir = opendir("F:\\image")) == NULL)
		return 0;

	entry = readdir(dir);
	entry = readdir(dir);
	int index = 0;
	while (!endFlag){
		sprintf_s(tmpStr, "%02d", stageNum);
		stageDirName[strlen(stageDirName) - 2] = '\0';
		strcat_s(stageDirName, tmpStr);

		if ((tmpDir = opendir(stageDirName)) == NULL){
			sprintf_s(sysStageDir, "mkdir %s", stageDirName);
			system(sysStageDir);
		}
		closedir(tmpDir);

		sprintf_s(dstName, stageDirName);
		int len = strlen(dstName);
		while (1){
			if (!(entry = readdir(dir))){
				endFlag = 1;
				break;
			}
			if (strstr(entry->d_name, ".jpg") ||
				strstr(entry->d_name, ".JPG") ||
				strstr(entry->d_name, ".jpeg") ||
				strstr(entry->d_name, ".JPEG") ||
				strstr(entry->d_name, ".png") ||
				strstr(entry->d_name, ".PNG")){
				sprintf_s(srcName, "F:\\image\\%s", entry->d_name);
				SrcImg = imread(srcName);
				entry = NULL;
			}
			else{
				entry = NULL;
				continue;
			}
			
			sprintf_s(sysRename, "rename %s image_%07d.jpg", srcName, imgBackupNum);
			system(sysRename);
			sprintf_s(srcName, "F:\\image\\image_%07d.jpg", imgBackupNum++);
			sprintf_s(sysMove, "move %s %s", srcName, "F:\\image_backup");
			system(sysMove);
			

			if (!(SrcImg.cols < size2.width || SrcImg.rows < size2.height))
				resize(SrcImg, newSrcImg, size2, INTER_AREA);
			else
				newSrcImg = SrcImg.clone();

			for (int x = 0; x + WIDTH <= newSrcImg.cols; x += WIDTH){
				for (int y = 0; y + HEIGHT <= newSrcImg.rows; y += HEIGHT){
					Rect rect = Rect(x, y, WIDTH, HEIGHT);
					Mat DstImg = newSrcImg(rect);

					sprintf_s(dstNum, "\\not_face_No_%06d.bmp", dstCount[stageNum - 1]++);
					strcat_s(dstName, dstNum);
					imwrite(dstName, DstImg);
					dstName[len] = '\0';

					DstImg.release();
				}
			}
			newSrcImg.release();
			resize(SrcImg, newSrcImg, size, INTER_AREA);
			sprintf_s(dstNum, "\\not_face_No_%06d.bmp", dstCount[stageNum - 1]++);
			strcat_s(dstName, dstNum);
			imwrite(dstName, newSrcImg);
			dstName[len] = '\0';

			newSrcImg.release();
			SrcImg.release();

			cout << "image" << ++index << " complete." << endl;
			if (dstCount[stageNum - 1] >= 100000){
				stageNum++;
				break;
			}
		}
	}

	rewind(config);
	fprintf_s(config, "%d\n", stageNum);
	for (int i = 0; i < stageNum; i++)
		fprintf_s(config, "%d\n", dstCount[i]);
	fclose(config);
	
	rewind(config_imgBackup);
	fprintf_s(config_imgBackup, "%d\n", imgBackupNum);
	fclose(config_imgBackup);
	
	return 0;
}