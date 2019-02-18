//
// Created by Jack Yu on 23/10/2017.
//

#include "../include/Pipeline.h"

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

template<class T>
static unsigned int levenshtein_distance(const T &s1, const T &s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

    for (unsigned int i = 0; i < prevCol.size(); i++) prevCol[i] = i;
    for (unsigned int i = 0; i < len1; i++) {
        col[0] = i + 1;
        for (unsigned int j = 0; j < len2; j++)
            col[j + 1] = min(
                    min(prevCol[1 + j] + 1, col[j] + 1),
                    prevCol[j] + (s1[i] == s2[j] ? 0 : 1));
        col.swap(prevCol);
    }
    return prevCol[len2];
}




void TEST_ACC(){

    pr::PipelinePR prc("/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/cascade.xml",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/HorizonalFinemapping.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/HorizonalFinemapping.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/Segmentation.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/Segmentation.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/CharacterRecognization.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/CharacterRecognization.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/SegmenationFree-Inception.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/SegmenationFree-Inception.caffemodel"
    );

    ifstream file;
    string imagename;
    int n = 0,correct = 0,j = 0,sum = 0;
    char filename[] = "/Users/yujinke/Downloads/general_test/1.txt";
    string pathh = "/Users/yujinke/Downloads/general_test/";
    file.open(filename, ios::in);
    while (!file.eof())
    {
        file >> imagename;
        string imgpath = pathh + imagename;
        std::cout << "------------------------------------------------" << endl;
        cout << "图片名：" << imagename << endl;
        cv::Mat image = cv::imread(imgpath);
//		cv::imshow("image", image);
//		cv::waitKey(0);

        std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image,pr::SEGMENTATION_FREE_METHOD);

        float conf = 0;
        vector<float> con ;
        vector<string> name;
        for (auto st : res) {
            if (st.confidence > 0.1) {
                //std::cout << st.getPlateName() << " " << st.confidence << std::endl;
                con.push_back(st.confidence);
                name.push_back(st.getPlateName());
                //conf += st.confidence;
            }
            else
                cout << "no string" << endl;
        }
        //	std::cout << conf << std::endl;
        int num = con.size();
        float max = 0;
        string platestr, chpr, ch;
        int diff = 0,dif = 0;
        for (int i = 0; i < num; i++) {

            if (con.at(i) > max)
            {
                max = con.at(i);
                platestr = name.at(i);
            }

        }
        //	cout << "max:"<<max << endl;
        cout << "string:" << platestr << endl;
        chpr = platestr.substr(0, 2);
        ch = imagename.substr(0, 2);
        diff = levenshtein_distance(imagename, platestr);
        dif = diff - 4;
        cout << "差距:" <<dif << endl;
        sum += dif;
        if (ch != chpr) n++;
        if (diff == 0)	correct++;
        j++;
    }
    float cha = 1 - float(n) / float(j);
    std::cout << "------------------------------------------------" << endl;
    cout << "车牌总数：" << j << endl;
    cout << "汉字识别准确率："<<cha << endl;
    float chaccuracy = 1 - float(sum - n * 2) /float(j * 8);
    cout << "字符识别准确率：" << chaccuracy << endl;

}

bool getImageName(const char *fileName, vector<string> &imageName)
{
    FILE *f = fopen(fileName, "r");
    if (f == NULL)
        return false;
    char buffer[300];
    while (fgets(buffer, 300, f))
    {
        //去掉换行符
        buffer[strlen(buffer) - 1] = '\0';
        imageName.push_back(string(buffer));
    }
    fclose(f);
    return true;
}


void TEST_ARGS_PIPELINE(int argc, char *argv[]){
    const std::string keys =
        "{help h|      | need help }"
        "{minNeighbors | 3 | minNeighbors }"
        "{scale | 1.1     | scale factor of detection }"
        "{scalew | 1.0 | detection scale in width range }"
        "{scaleh | 1.0 | detection scale in height range }"
        "{cascade_path | | cascade xml of detection model.}"
        "{img_path | | image folder to be detect }"
        "{rst_path | | result folder to save images }"
        "{img_list | | image list to be detect }"
        ;
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Test Pipeline");
    if (parser.has("help")){
        parser.printMessage();
        return;
    }

    int minNeighbors = parser.get<int>("minNeighbors");
    std::string cascade_path = parser.get<std::string>("cascade_path");
    std::string img_path = parser.get<std::string>("img_path");
    std::string rst_path = parser.get<std::string>("rst_path");
    std::string img_list = parser.get<std::string>("img_list");

    float scale = parser.get<float>("scale");
    float scalew = parser.get<float>("scalew");
    float scaleh = parser.get<float>("scaleh");

    std::cout << "scale: " << scale << std::endl;
    std::cout << "scalew: " << scalew << std::endl;
    std::cout << "scaleh: " << scaleh << std::endl;
    std::cout << "minNeighbors: " << minNeighbors << std::endl;
    std::cout << "cascade_path: " << cascade_path << std::endl;
    std::cout << "img_path: " << img_path.c_str() << std::endl;
    std::cout << "rst_path: " << rst_path.c_str() << std::endl;
    std::cout << "img_list: " << img_list.c_str() << std::endl;

    pr::PipelinePR prc(cascade_path, 
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/HorizonalFinemapping.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/HorizonalFinemapping.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/Segmentation.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/Segmentation.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/CharacterRecognization.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/CharacterRecognization.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/SegmenationFree-Inception.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/SegmenationFree-Inception.caffemodel"
    );

    int font_face = cv::FONT_HERSHEY_PLAIN; 
    double font_scale = 1;
    int thickness = 1;
    int baseline;
    std::vector<std::string> imageName;

    if (!getImageName(img_list.c_str(), imageName)){
        std::cerr << "Can't open image_list.list file" << std::endl;
    }
    
    for(int i=0;i<imageName.size();i++){
        cv::Mat image = cv::imread(img_path + imageName.at(i));
        std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image,pr::SEGMENTATION_FREE_METHOD);
        for(auto st:res) {
            if(st.confidence>0.4) {
                std::cout << st.getPlateName() << " " << st.confidence << std::endl;
                cv::Rect region = st.getPlateRect();

                cv::rectangle(image,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),2);
                cv::putText(image, st.getPlateName(), cv::Point(region.x,region.y), font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 8, 0);

            }
        }
        cv::imwrite(rst_path+imageName.at(i),image);
    }
}
void TEST_PIPELINE(){

pr::PipelinePR prc("/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/cascade.xml",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/HorizonalFinemapping.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/HorizonalFinemapping.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/Segmentation.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/Segmentation.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/CharacterRecognization.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/CharacterRecognization.caffemodel",
                       "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/SegmenationFree-Inception.prototxt","/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/SegmenationFree-Inception.caffemodel"
    );
    int font_face = cv::FONT_HERSHEY_PLAIN; 
    double font_scale = 1;
    int thickness = 1;
    int baseline;
    std::vector<string> imageName;
    if (!getImageName("/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/img.txt", imageName))
    {
        cerr << "Can't open image_list.list file" << endl;
    }
    std::string imgdir = "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/build/card_0215/";
    std::string resultdir = "/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/build/result/";
    for(int i=0;i<imageName.size();i++)
    {
    cv::Mat image = cv::imread(imgdir + imageName.at(i));
    double timeStart = (double)getTickCount();
    std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image,pr::SEGMENTATION_FREE_METHOD);
    double DetectionTime = ((double)getTickCount() - timeStart) / getTickFrequency()*1000;
    std::cout<<"Total time: "<<DetectionTime<<" Ms"<<std::endl;
    for(auto st:res) {
        if(st.confidence>0.4) {
            std::cout << st.getPlateName() << " " << st.confidence << std::endl;
            cv::Rect region = st.getPlateRect();

            cv::rectangle(image,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),2);
            cv::putText(image, st.getPlateName(), cv::Point(region.x,region.y), font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 8, 0);

        }
    }
    cv::imwrite(resultdir+imageName.at(i),image);
    }
    //cv::imshow("image",image);
    //cv::waitKey(0);
    //cv::imwrite("result.jpg",image);

}


void TEST_CAM()
{

    cv::VideoCapture capture("test1.mp4");
    cv::Mat frame;

    pr::PipelinePR prc("model/cascade.xml",
                       "model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel",
                       "model/Segmentation.prototxt","model/Segmentation.caffemodel",
                       "model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel",
                       "model/SegmentationFree.prototxt","model/SegmentationFree.caffemodel"
    );





    while(1) {
        //读取下一帧
        if (!capture.read(frame)) {
            std::cout << "读取视频失败" << std::endl;
            exit(1);
        }
//
//        cv::transpose(frame,frame);
//        cv::flip(frame,frame,2);

//        cv::resize(frame,frame,cv::Size(frame.cols/2,frame.rows/2));



        std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(frame,pr::SEGMENTATION_FREE_METHOD);

        for(auto st:res) {
            if(st.confidence>0.75) {
                std::cout << st.getPlateName() << " " << st.confidence << std::endl;
                cv::Rect region = st.getPlateRect();

                cv::rectangle(frame,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),2);
            }
        }

        cv::imshow("image",frame);
        cv::waitKey(1);



    }

}


int main(int argc, char *argv[])
{
//  TEST_ACC();
//  TEST_CAM();
//  TEST_PIPELINE();

    TEST_ARGS_PIPELINE(argc, argv);

    return 0 ;


}
