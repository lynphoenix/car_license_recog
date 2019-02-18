//
// Created by Jack Yu on 23/10/2017.
//

#include "../include/Pipeline.h"

#include <opencv2/opencv.hpp>
using namespace cv;
namespace pr {



    const int HorizontalPadding = 4;
    PipelinePR::PipelinePR(std::string detector_filename,
                           std::string finemapping_prototxt, std::string finemapping_caffemodel,
                           std::string segmentation_prototxt, std::string segmentation_caffemodel,
                           std::string charRecognization_proto, std::string charRecognization_caffemodel,
                           std::string segmentationfree_proto,std::string segmentationfree_caffemodel) {
        plateDetection = new PlateDetection(detector_filename);
        fineMapping = new FineMapping(finemapping_prototxt, finemapping_caffemodel);
        plateSegmentation = new PlateSegmentation(segmentation_prototxt, segmentation_caffemodel);
        generalRecognizer = new CNNRecognizer(charRecognization_proto, charRecognization_caffemodel);
        segmentationFreeRecognizer =  new SegmentationFreeRecognizer(segmentationfree_proto,segmentationfree_caffemodel);

    }

    PipelinePR::~PipelinePR() {

        delete plateDetection;
        delete fineMapping;
        delete plateSegmentation;
        delete generalRecognizer;
        delete segmentationFreeRecognizer;

    }

    std::vector<PlateInfo> PipelinePR:: RunPiplineAsImage(cv::Mat plateImage, int method,
                                                          float scalew, float scaleh,
                                                          float scalefactor, minNeighbors,
                                                          float recog_conf,
                                                          std::string rst_det_path,
                                                          std::string rst_seg_path,
                                                          std::string rst_rcg_path,
                                                          std::string imageName,
                                                          int& totalRects, int& totalLicences,
                                                          float& totalDetTime, float& totalRcgTime
                                                          ) {
        std::vector<PlateInfo> results;
        std::vector<PlateInfo> plates;

        double timeStart = (double)getTickCount();

        int x = int(float(plateImage.cols) * (1.0 - scalew));
        int y = int(float(plateImage.rows) * (1.0 - scaleh));

        plateDetection->plateDetectionRough(plateImage,plates,36,700,
                                            cv::Point(x,y), 
                                            cv::Size2f(scalew, scaleh), 
                                            scale, minNeighbors);

        double DetectionTime = ((double)getTickCount() - timeStart) / getTickFrequency()*1000;
        
        for (pr::PlateInfo plateinfo:plates) {
            cv::Mat image_finemapping = plateinfo.getPlateImage();
            image_finemapping = fineMapping->FineMappingVertical(image_finemapping);
            image_finemapping = pr::fastdeskew(image_finemapping, 5);

            //Segmentation-based
            if(method==SEGMENTATION_BASED_METHOD)
            {
                image_finemapping = fineMapping->FineMappingHorizon(image_finemapping, 2, HorizontalPadding);
                cv::resize(image_finemapping, image_finemapping, cv::Size(136+HorizontalPadding, 36));
                plateinfo.setPlateImage(image_finemapping);
                std::vector<cv::Rect> rects;
                plateSegmentation->segmentPlatePipline(plateinfo, 1, rects);
                plateSegmentation->ExtractRegions(plateinfo, rects);
                cv::copyMakeBorder(image_finemapping, image_finemapping, 0, 0, 0, 20, cv::BORDER_REPLICATE);
                plateinfo.setPlateImage(image_finemapping);
                generalRecognizer->SegmentBasedSequenceRecognition(plateinfo);
                plateinfo.decodePlateNormal(pr::CH_PLATE_CODE);

            }
            //Segmentation-free
            else if(method==SEGMENTATION_FREE_METHOD)
            {
                image_finemapping = fineMapping->FineMappingHorizon(image_finemapping, 4, HorizontalPadding+3);
                cv::resize(image_finemapping, image_finemapping, cv::Size(136+HorizontalPadding, 36));
                plateinfo.setPlateImage(image_finemapping);
                std::pair<std::string,float> res = segmentationFreeRecognizer->SegmentationFreeForSinglePlate(plateinfo.getPlateImage(),pr::CH_PLATE_CODE);
                plateinfo.confidence = res.second;
                plateinfo.setPlateName(res.first);
            }
            results.push_back(plateinfo);
        }
        double RecognitionTime = ((double)getTickCount() - timeStart) / getTickFrequency()*1000 - DetectionTime;
        std::cout<<"Detection time: "<<DetectionTime<<" Ms, with " << plates.size() << " plates got. \t";
        std::cout<<"Recognition time: "<<RecognitionTime<<" Ms, with " << results.size() << " licences got."<<std::endl;
        for (pr::PlateInfo plateinfo:plates) {
            cv::Rect region = plateinfo.getPlateRect();
            cv::rectangle(plateImage,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,0,0),3);
        }
        cv::imwrite(rst_det_path+imageName,plateImage);
        for(auto st:results) {
            if(st.confidence>recog_conf) {
                std::cout << st.getPlateName() << " " << st.confidence << std::endl;
                cv::Rect region = st.getPlateRect();

                cv::rectangle(plateImage,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),1);
                cv::putText(plateImage, st.getPlateName(), cv::Point(region.x,region.y), font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 8, 0);

            }
        }
        cv::imwrite(rst_rcg_path+imageName,plateImage);

        totalRects += plates.size();
        totalLicences += results.size();
        totalDetTime += DetectionTime;
        totalRcgTime += RecognitionTime;

        return results;

    }//namespace pr



}
