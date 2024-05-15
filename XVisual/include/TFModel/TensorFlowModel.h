#ifndef TensorFlowModel_H
#define TensorFlowModel_H

#include <filesystem>
#include <fstream>
#include "Common/OpenCVHeaders.h"
#include "tensorflow/c/c_api.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"
#include "tensorflow/core/platform/protobuf.h"
#include "Common/StrUtils.h"
#include "TFModel/TFType2CPPType.h"
#include "Common/BoxUtils.h"
#include "TFModel/ModelUtils.h"
#include "Common/DetectResult.h"

class TensorFlowModel
{
	private:
		TF_Status* status_ = nullptr;
		TF_Session* session_ = nullptr;
		TF_Graph* graph_ = nullptr;
		TF_SessionOptions* session_opts_ = nullptr;
		TF_Buffer* run_options_ = nullptr;
		TF_Buffer* metagraph_ = nullptr;
		const char* tag1_ = "serve";
		const char* const tags_[1] = { tag1_ };
		int num_tags_ = 1;
		int NumInputs_ = 1;
		int NumOutputs_ = 1;

		std::string input_name_ = "";
		std::string output_name_ = "";

		TF_Output* inputs_ = nullptr;
		TF_Output* outputs_ = nullptr;
		TF_Tensor** InputValues_ = nullptr;
		TF_Tensor** OutputValues_ = nullptr;
		float* input_data_;

		std::string input_key_ = "";
		std::string inputType1Str_ = "";
		std::string inputShape1Str_ = "";
		std::string inputName1Str_ = "";

		std::string output_key_ = "";
		std::string outputType1Str_ = "";
		std::string outputShape1Str_ = "";
		std::string outputName1Str_ = "";

		// image_width_, image_height_ is expected to match the input size of TFModel
		int image_height_ = 416; // When TensorFlowModel::loadInputData is called, image_height_ will be updated
		int image_width_ = 416; // When TensorFlowModel::loadInputData is called, image_width_ will be updated

	public:
		TensorFlowModel();

		~TensorFlowModel();

		void printOperations(const std::string& tfOperNameRecordDir);

		bool loadSavedModel(const std::string& saved_model_dir);

		void loadInputData(cv::Mat image);

		void doRun();

		// The coordinate size of the Results obtained here is relative to the model input scale, not to the original image size
		void getResults(std::vector<DetectResult >& results);
};



#endif //TensorFlowModel_H

