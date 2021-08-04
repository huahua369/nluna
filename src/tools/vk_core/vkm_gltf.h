#pragma once
namespace vkm
{
	class dvk_gltf
	{
	public:
		dvk_gltf()
		{
		}

		~dvk_gltf()
		{
		}
		void* load2file(const std::string& fn);
	private:
		unsigned int version;       // 4 bytes
		unsigned int length;        // 4 bytes
		unsigned int model_length;  // 4 bytes
		unsigned int model_format;  // 4 bytes;
		njson info;
		bool is_binary = false;
		char* bin_data = 0;  // 4 bytes (buffer_length) + 4 bytes(buffer_format)
		uint64_t bin_size = 0;
		std::vector<char> rawdata;
	};

}
