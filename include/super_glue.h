//
// Created by haoyuefan on 2021/9/22.
//

#ifndef SUPER_GLUE_H_
#define SUPER_GLUE_H_

#include <string>
#include <memory>
#include <NvInfer.h>
#include <Eigen/Core>
#include <NvOnnxParser.h>
#include <opencv2/opencv.hpp>

#include "3rdparty/tensorrtbuffer/include/buffers.h"
#include "read_config.h"

using tensorrt_common::TensorRTUniquePtr;

class SuperGlue {
public:
    explicit SuperGlue(const SuperGlueConfig &superglue_config);

    bool build();

    bool infer(const Eigen::Matrix<double, 259, Eigen::Dynamic> &features0,
               const Eigen::Matrix<double, 259, Eigen::Dynamic> &features1,
               Eigen::VectorXi &indices0,
               Eigen::VectorXi &indices1,
               Eigen::VectorXd &mscores0,
               Eigen::VectorXd &mscores1);


    int matching_points(Eigen::Matrix<double, 259, Eigen::Dynamic> &features0, Eigen::Matrix<double, 259,
            Eigen::Dynamic> &features1, std::vector<cv::DMatch> &matches, bool outlier_rejection = false);

    Eigen::Matrix<double, 259, Eigen::Dynamic> normalize_keypoints(
            const Eigen::Matrix<double, 259, Eigen::Dynamic> &features, int width, int height);

    void save_engine();

    bool deserialize_engine();

private:
    SuperGlueConfig superglue_config_;
    std::vector<int> indices0_;
    std::vector<int> indices1_;
    std::vector<double> mscores0_;
    std::vector<double> mscores1_;

    nvinfer1::Dims keypoints_0_dims_{};
    nvinfer1::Dims scores_0_dims_{};
    nvinfer1::Dims descriptors_0_dims_{};
    nvinfer1::Dims keypoints_1_dims_{};
    nvinfer1::Dims scores_1_dims_{};
    nvinfer1::Dims descriptors_1_dims_{};
    nvinfer1::Dims output_scores_dims_{};

    std::shared_ptr<nvinfer1::ICudaEngine> engine_;
    std::shared_ptr<nvinfer1::IExecutionContext> context_;

    bool construct_network(TensorRTUniquePtr<nvinfer1::IBuilder> &builder,
                           TensorRTUniquePtr<nvinfer1::INetworkDefinition> &network,
                           TensorRTUniquePtr<nvinfer1::IBuilderConfig> &config,
                           TensorRTUniquePtr<nvonnxparser::IParser> &parser) const;

    bool process_input(const tensorrt_buffer::BufferManager &buffers,
                       const Eigen::Matrix<double, 259, Eigen::Dynamic> &features0,
                       const Eigen::Matrix<double, 259, Eigen::Dynamic> &features1);

    bool process_output(const tensorrt_buffer::BufferManager &buffers,
                        Eigen::VectorXi &indices0,
                        Eigen::VectorXi &indices1,
                        Eigen::VectorXd &mscores0,
                        Eigen::VectorXd &mscores1);
                        
    bool create_default_optimization_profile(TensorRTUniquePtr<nvinfer1::IBuilder> &builder, TensorRTUniquePtr<nvinfer1::IBuilderConfig> &config);
    bool create_optimization_profile_with_input_sizes(TensorRTUniquePtr<nvinfer1::IBuilder> &builder, TensorRTUniquePtr<nvinfer1::IBuilderConfig> &config, int num_keypoints_0, int num_keypoints_1);
    bool create_multiple_optimization_profiles(TensorRTUniquePtr<nvinfer1::IBuilder> &builder, TensorRTUniquePtr<nvinfer1::IBuilderConfig> &config);

};

typedef std::shared_ptr<SuperGlue> SuperGluePtr;

#endif //SUPER_GLUE_H_
