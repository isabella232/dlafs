/*
 *Copyright (C) 2018 Intel Corporation
 *
 *SPDX-License-Identifier: LGPL-2.1-only
 *
 *This library is free software; you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation;
 * version 2.1.
 *
 *This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library;
 * if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
  * Author: River,Li
  * Email: river.li@intel.com
  * Date: 2018.10
  */


#ifndef __IE_LOADER_H__
#define __IE_LOADER_H__

#include <string>
#include <vector>
#include <map>
#include <vector>
#include <tuple>
#include <mutex>
#include <condition_variable>
#include <opencv2/opencv.hpp>
#include <inference_engine.hpp>
//#include "algobase.h"

#ifndef CVDL_MODEL_DIR_DEFAULT
// model directory, alt it can be passed from app
#define CVDL_MODEL_DIR_DEFAULT "/usr/lib/x86_64-linux-gnu/libgstcvdl"
#endif

#define CHECK(X) if(!(X)){ GST_ERROR("CHECK ERROR!"); std::exit(EXIT_FAILURE); }
#define REQUEST_NUM 16


enum{
    IE_MODEL_DETECTION = 0,
    IE_MODEL_CLASSFICATION = 1,
    IE_MODEL_SSD =2,
    IE_MODEL_LP_RECOGNIZE = 3,
    IE_MODEL_YOLOTINYV2 = 4,
    IE_MODEL_REID = 5,
    IE_MODEL_GENERIC = 6, 
    IE_MODEL_LP_NONE
};

using AsyncCallback = std::function<void(void* algoData)>;

class IELoader {
public:
    IELoader();
    ~IELoader();

    GstFlowReturn set_device(InferenceEngine::TargetDevice dev);
    GstFlowReturn read_model(std::string strModelXml, std::string strModelBin, int modelType, std::string network_config);
    GstFlowReturn convert_input_to_blob(const cv::UMat& img, InferenceEngine::Blob::Ptr& inputBlobPtr);

    // move to algoBase
    // parse inference result for a frame, which may contain mutiple objects
    //virtual GstFlowReturn parse_inference_result(InferenceEngine::Blob::Ptr &resultBlobPtr, int precision,
    //                                                    CvdlAlgoData *outData, int objId);
    GstFlowReturn do_inference_async(void *algoData, uint64_t frmId, int objId,
                                            cv::UMat &src, AsyncCallback cb);
    GstFlowReturn do_inference_sync(void *data, uint64_t frmId, int objId,
                                                  cv::UMat &src);
    GstFlowReturn get_input_size(int *w, int *h, int *c);
    GstFlowReturn get_out_size(int *outDim0, int *outDim1);
    // must be called before read_model()
    void set_precision(InferenceEngine::Precision in, InferenceEngine::Precision out)
    {
        mInputPrecision = in;
        mOutputPrecision = out;
    }
    //
    int mModelType;

    int mOutputDim[2];

    InferenceEngine::TargetDevice mTargetDev;
    InferenceEngine::Precision mInputPrecision = InferenceEngine::Precision::U8;
    InferenceEngine::Precision mOutputPrecision = InferenceEngine::Precision::FP32;

    // mean/scale for input, which is used convert u8 image data to float data
    // make sure mInputPrecision=InferenceEngine::Precision::FP32
    float mInputMean;
    float mInputScale;
    void set_mean_and_scale(float mean, float scale)
    {
          mInputMean = mean;
          mInputScale = scale;
    }
private:
    int get_enable_request();
    void release_request(int reqestId);

    std::string mModelXml;
    std::string mModelBin;

    InferenceEngine::InferenceEnginePluginPtr mIEPlugin; // plugin must be first so it would be last in the destruction order
    InferenceEngine::IExecutableNetwork::Ptr  mExeNetwork;

    std::string mFirstInputName;
    std::string mFirstOutputName;

    std::mutex mRequstMutex;
    std::condition_variable mCondVar;
   
    // <request, used or not>
    InferenceEngine::IInferRequest::Ptr mInferRequest[REQUEST_NUM];
    bool mRequestEnable[REQUEST_NUM] = {true};
};

#endif