//
// Created by 庾金科 on 20/09/2017.
//

#include <../include/PlateDetection.h>
#include<iostream>
#include<string.h>

void drawRect(cv::Mat image,cv::Rect rect)
{
    cv::Point p1(rect.x,rect.y);
    cv::Point p2(rect.x+rect.width,rect.y+rect.height);
    cv::rectangle(image,p1,p2,cv::Scalar(0,255,0),1);
}


bool getImageName(const char *fileName, std::vector<std::string> &imageName)
{
    FILE *f = fopen(fileName, "r");
    if (f == NULL)
        return false;
    char buffer[300];
    while (fgets(buffer, 300, f))
    {
        //去掉换行符
        buffer[strlen(buffer) - 1] = '\0';
        imageName.push_back(std::string(buffer));
    }
    fclose(f);
    return true;
}


int main(int argc, char *argv[])
{
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
    parser.about("Test Detection");
    if (parser.has("help")){
        parser.printMessage();
        return 0;
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

    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    std::vector<std::string> imageName;
    if (!getImageName("/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/img.txt", imageName))
    {
        std::cerr << "Can't open image_list.list file" << std::endl;
    }

    pr::PlateDetection plateDetection(cascade_path);
    int totalRects = 0;
    float totalTime = 0.f;
    for(int i=0;i<imageName.size();i++){
        std::vector<pr::PlateInfo> plates;
        cv::Mat image = cv::imread(img_path + imageName.at(i));
        int x = int(float(image.cols) * (1.0 - scalew));
        int y = int(float(image.rows) * (1.0 - scaleh));
        double timeStart = (double)cv::getTickCount();
        plateDetection.plateDetectionRough(image,plates,36,700, 
                                           cv::Point(x,y), 
                                           cv::Size2f(scalew, scaleh), 
                                           scale, minNeighbors);
        double DetectionTime = ((double)cv::getTickCount() - timeStart) / cv::getTickFrequency()*1000;
        totalRects += plates.size();
        totalTime += DetectionTime;
        std::cout<<"Total time: "<<DetectionTime<<" Ms"<<std::endl;
        for(pr::PlateInfo platex:plates)
        {
            drawRect(image,platex.getPlateRect());
            cv::imwrite(rst_path+imageName.at(i),platex.getPlateImage());
        }

    }
    std::cout<<"Total Plates: "<<totalRects<<". Counts."<<std::endl;
    std::cout<<"Total Detection Time: "<<totalTime<<" ms."<<std::endl;

    return 0 ;


}
