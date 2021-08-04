

#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif /*_WIN32*/
#include <base/camera.hpp>
#include "model.h"
#include <view/file.h>
#include "vkm_gltf.h"
#include "tiny_gltf.h"
//#include <simdjson.h>
namespace vkm {

	static std::string GetBaseDir(const std::string &filepath) {
		if (filepath.find_last_of("/\\") != std::string::npos)
			return filepath.substr(0, filepath.find_last_of("/\\"));
		return "";
	}
	static void swap4(unsigned int* val) {
#ifndef VKM_LITTLE_ENDIAN
		(void)val;
#else
		unsigned int tmp = *val;
		unsigned char* dst = reinterpret_cast<unsigned char*>(val);
		unsigned char* src = reinterpret_cast<unsigned char*>(&tmp);

		dst[0] = src[3];
		dst[1] = src[2];
		dst[2] = src[1];
		dst[3] = src[0];
#endif
	}

	void* dvk_gltf::load2file(const std::string& fn)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;


		/*	if (!warn.empty()) {
				printf("Warn: %s\n", warn.c_str());
			}

			if (!err.empty()) {
				printf("Err: %s\n", err.c_str());
			}

			if (!ret) {
				printf("Failed to parse glTF\n");
				return -1;
			}*/
		bool ret = false;
		rawdata = hz::File::read_binary_file(fn.c_str());
		if (rawdata.size() < 20)
			return 0;
		char* bytes = rawdata.data();
		auto size = rawdata.size();

		std::string basedir = GetBaseDir(fn);
		// for binary glTF(.glb)
		ret = loader.LoadBinaryFromMemory(&model, &err, &warn, (unsigned char*)&rawdata[0], size, basedir);

		if (!ret) {
			ret = loader.LoadASCIIFromString(&model, &err, &warn, (char*)&rawdata[0], size, basedir);
		}

		// @todo { Endian swap for big endian machine. }
		memcpy(&version, bytes + 4, 4);
		swap4(&version);
		memcpy(&length, bytes + 8, 4);
		swap4(&length);
		memcpy(&model_length, bytes + 12, 4);
		swap4(&model_length);
		memcpy(&model_format, bytes + 16, 4);
		swap4(&model_format);

		// In case the Bin buffer is not present, the size is exactly 20 + size of
		// JSON contents,
		// so use "greater than" operator.
		if ((20 + model_length > size) || (model_length < 1) || (length > size) ||
			(20 + model_length > length) ||
			(model_format != 0x4E4F534A)) {  // 0x4E4F534A = JSON format.
			printf("Invalid glTF binary.");
			return 0;
		}

		// Extract JSON string.
		std::string jsonString(reinterpret_cast<const char*>(&bytes[20]),
							   model_length);

		is_binary = true;
		bin_data = bytes + 20 + model_length + 8;  // 4 bytes (buffer_length) + 4 bytes(buffer_format)
		bin_size = length - (20 + model_length);  // extract header + JSON scene data.
		char* jsonstr = (char*)(&bytes[20]);
		info = njson::parse(jsonstr, jsonstr + model_length);
		return nullptr;
	}

}
//!vkm
