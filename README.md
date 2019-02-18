# car_license_recog

### 测试脚本
* 测试车牌检测

../lpr/TEST_Detection \
    --scaleh=0.5 \
    --scalew=1.0 \
    --minNeighbors=2 \
    --cascade_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/cascade.xml" \
    --img_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/build/card_0215/" \
    --rst_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/build/result-halfsize/" \
    --img_list="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/img.txt"

../lpr/TEST_Detection \
    --minNeighbors=2 \
    --cascade_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/cascade.xml" \
    --img_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/build/card_0215/" \
    --rst_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/build/result-fullsize/" \
    --img_list="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/img.txt"

* 测试端到端车牌识别Pipeline
../lpr/TEST_PIPELINE \
    --minNeighbors=1 \
    --scaleh=1.0 \
    --scalew=1.0 \
    --recog_conf=0.1 \
    --cascade_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/model/cascade.xml" \
    --img_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/build/card_0215/" \
    --rst_path="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/build/result-fullsize/" \
    --img_list="/disk1/huajianni/temp/HyperLPR-master/Prj-Linux/lpr/img.txt"
